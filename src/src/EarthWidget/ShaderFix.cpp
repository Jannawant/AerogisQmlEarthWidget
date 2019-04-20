#include "ShaderFix.h"

#include <osg/Notify>
#include <osg/DisplaySettings>
#include <osgEarthUtil/ExampleResources>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarthUtil/Controls>
#include <osgEarth/Registry>
#include <osgEarth/Capabilities>
#include <osgEarth/MapNode>
#include <QMessageBox>
#include <osgEarth/SpatialReference>
#include <osgGA/TrackballManipulator>
#include <osgEarthUtil/Sky>
#include <osgEarthSymbology/Geometry>
#include "QuickWindowViewer.hpp"
#include <set>

#include <osgEarth/VirtualProgram>

#include <osgEarth/ShaderFactory>
#include <osgEarth/ShaderUtils>
#include <osgEarth/StringUtils>
#include <osgEarth/Containers>
#include <osg/Shader>
#include <osg/Program>
#include <osg/State>
#include <osg/Geode>
#include <osg/Version>
#include <osg/GL2Extensions>
#include <osg/GLExtensions>
#include <fstream>
#include <sstream>
#include <OpenThreads/Thread>

#include <osg/PositionAttitudeTransform>

#include <osgEarthDrivers/engine_mp/MPTerrainEngineNode>

#include <osgEarthDrivers/engine_mp/SingleKeyNodeFactory>
#include <osgEarthDrivers/engine_mp/TerrainNode>
#include <osgEarthDrivers/engine_mp/TileModelFactory>
#include <osgEarthDrivers/engine_mp/TileModelCompiler>
#include <osgEarthDrivers/engine_mp/TilePagedLOD>
#include <osgEarthDrivers/engine_mp/MPShaders>

#include <osgEarth/HeightFieldUtils>
#include <osgEarth/ImageUtils>
#include <osgEarth/MapModelChange>
#include <osgEarth/Progress>
#include <osgEarth/ShaderLoader>
#include <osgEarth/Utils>
#include <osgEarth/ObjectIndex>

#include <osg/TexEnv>
#include <osg/TexEnvCombine>
#include <osg/PagedLOD>
#include <osg/Timer>
#include <osg/Depth>
#include <osg/BlendFunc>
#include <osgDB/DatabasePager>
#include <osgUtil/RenderBin>
#include <osgUtil/RenderLeaf>
#include "osg/Geometry"
#include <qopenglcontext.h>
#include <qopenglfunctions.h>



#define BIND_VERTEX 8
#define BIND_NORMAL 9

void shReplace(osg::Shader*sh, std::string _what, std::string _to)
{
   std::string src = sh->getShaderSource();
   auto pos = src.find(_what);
   if (pos != std::string::npos)
   {
      src.replace(pos, _what.length(), _to);
      sh->setShaderSource(src);
   }
}

size_t find_strict(std::string &src, std::string _what)
{
    auto _pos = src.find(_what);
    while( _pos != std::string::npos && isalpha(src[_pos+_what.length()]) )
    {
        _pos = src.find(_what,_pos+_what.length());
    }
    return _pos;
}

void shReplaceAttribute(osg::Shader *sh, std::string nat_at, std::string need_at, std::string osg_at,
                        std::string need_def,std::string osg_def )
{
    std::string src = sh->getShaderSource();
    std::string lsp = nat_at;
    std::string nlsp = need_at;
    auto ls_pos = find_strict(src,lsp);
    if (ls_pos != std::string::npos)
    {
       while (ls_pos != std::string::npos)
       {
          src.replace(ls_pos, lsp.length(), nlsp);
          ls_pos = find_strict(src,lsp);
       }

       ls_pos = 0;
       if (src.substr(0, 8) == "#version")
          ls_pos = src.find('\n') + 1;
       src.insert(ls_pos, need_def+";\n");
    }
    lsp = osg_def;
    nlsp = need_def;
    ls_pos = find_strict(src,lsp);
    if (ls_pos != std::string::npos)
    {
        src.replace(ls_pos, lsp.length(), nlsp);
        lsp = osg_at;
        nlsp = need_at;
        ls_pos = find_strict(src,lsp);
        while (ls_pos != std::string::npos)
        {
            src.replace(ls_pos, lsp.length(), nlsp);
            ls_pos = find_strict(src,lsp);
        }
    }
    sh->setShaderSource(src);
}

void fixShader(osg::Shader *sh)
{
   shReplace(sh, "ftransform()", "gl_ModelViewProjectionMatrix * gl_Vertex");
   shReplace(sh, "gl_LightSource[0].position", "vec4(0,0,1,0)");
   shReplace(sh, "gl_FrontLightModelProduct.sceneColor", "vec4(0.2,0.2,0.2,1)");
   shReplace(sh, "gl_FrontLightProduct[0].ambient", "vec4(0,0,0,1)");
   shReplace(sh, "gl_FrontLightProduct[0].diffuse", "vec4(0.8,0.8,0.8,1)");
   shReplace(sh, "gl_FrontLightProduct[0].specular.rgb", "vec3(1,1,1)");
   shReplace(sh, "gl_FrontMaterial.shininess", "1.0");
}

void fixProgram(osg::Program *pp)
{
   auto bl = pp->getAttribBindingList();
   if (bl.find("a_Vertex") == bl.end())
      pp->addBindAttribLocation("a_Vertex", BIND_VERTEX);
   if (bl.find("a_Normal") == bl.end())
      pp->addBindAttribLocation("a_Normal", BIND_NORMAL);
   int shn = pp->getNumShaders();
   for (int shi = 0; shi<shn; shi++)
   {
      auto *sh = pp->getShader(shi);
      if (sh)
      {
         fixShader(sh);
      }
   }
}



#define USE_STACK_MEMORY 1

#define LC "[VirtualProgram] "

namespace {
   using namespace osgEarth;
   using namespace osgEarth::ShaderComp;

#ifdef OSG_GLES2_AVAILABLE
   // GLES requires all shader code be merged into a since source
   bool s_mergeShaders = true;
#else
   bool s_mergeShaders = false;
#endif

   bool s_dumpShaders = false;        // debugging

   /** A device that lets us do a const search on the State's attribute map. OSG does not yet
   have a const way to do this. It has getAttributeVec() but that is non-const (it creates
   the vector if it doesn't exist); Newer versions have getAttributeMap(), but that does not
   go back to OSG 3.0. */
   struct StateEx : public osg::State
   {
      static const VirtualProgram::AttrStack* getProgramStack(const osg::State& state)
      {
         static osg::StateAttribute::TypeMemberPair pair(VirtualProgram::SA_TYPE, 0);
         const StateEx* sh = reinterpret_cast<const StateEx*>(&state);
         AttributeMap::const_iterator i = sh->_attributeMap.find(pair);
         return i != sh->_attributeMap.end() ? &(i->second.attributeVec) : 0L;
      }
   };

   typedef std::map<std::string, std::string> HeaderMap;

   // removes leading and trailing whitespace, and replaces all other
   // whitespace with single spaces
   std::string trimAndCompress(const std::string& in)
   {
      bool inwhite = true;
      std::stringstream buf;
      for (unsigned i = 0; i<in.length(); ++i)
      {
         char c = in.at(i);
         if (::isspace(c))
         {
            if (!inwhite)
            {
               buf << ' ';
               inwhite = true;
            }
         }
         else
         {
            inwhite = false;
            buf << c;
         }
      }
      std::string r;
      r = buf.str();
      return trim(r);
   }


   void parseShaderForMerging(const std::string& source, unsigned& version, HeaderMap& headers, std::stringstream& body)
   {
      // break into lines:
      StringVector lines;
      StringTokenizer(source, lines, "\n", "", true, false);

      for (StringVector::const_iterator line_iter = lines.begin(); line_iter != lines.end(); ++line_iter)
      {
         std::string line = trimAndCompress(*line_iter);

         if (line.size() > 0)
         {
            StringVector tokens;
            StringTokenizer(line, tokens, " \t", "", false, true);

            if (tokens[0] == "#version")
            {
               // find the highest version number.
               if (tokens.size() > 1)
               {
                  unsigned newVersion = osgEarth::as<unsigned>(tokens[1], 0);
                  if (newVersion > version)
                  {
                     version = newVersion;
                  }
               }
            }

            else if (
               tokens[0] == "#extension" ||
               tokens[0] == "#define" ||
               tokens[0] == "precision" ||
               tokens[0] == "struct" ||
               tokens[0] == "varying" ||
               tokens[0] == "uniform" ||
               tokens[0] == "attribute")
            {
               std::string& header = headers[line];
               header = line;
            }

            else if (tokens[0] == "#pragma")
            {
               // Discards all pragmas, since the double-quotes in them are illegal in at least
               // GLSL ES compiler (on Android). We should consider doing this for all GLSL
               // since technically quoting characters are not part of the GLSL grammar at all.
               continue;
            }

            else
            {
               body << (*line_iter) << "\n";
            }
         }
      }
   }


   bool s_attribAliasSortFunc(const std::pair<std::string, std::string>& a, const std::pair<std::string, std::string>& b) {
      return a.first.size() > b.first.size();
   }


   /**
   * Replaces a shader's attribute values with their aliases
   */
   void applyAttributeAliases(
      osg::Shader*                             shader,
      const VirtualProgram::AttribAliasVector& sortedAliases)
   {
      std::string src = shader->getShaderSource();
      for (VirtualProgram::AttribAliasVector::const_iterator i = sortedAliases.begin(); i != sortedAliases.end(); ++i)
      {
         //OE_DEBUG << LC << "Replacing " << i->first << " with " << i->second << std::endl;
         osgEarth::replaceIn(src, i->first, i->second);
      }
      shader->setShaderSource(src);
   }


   /**
   * Adds a new shader entry to the accumulated shader map, respecting the
   * override policy of both the existing entry (if there is one) and the
   * new entry.
   */
   void addToAccumulatedMap(VirtualProgram::ShaderMap&         accumShaderMap,
      const VirtualProgram::ShaderID&    shaderID,
      const VirtualProgram::ShaderEntry& newEntry)
   {

      // see if we're trying to disable a previous entry:
      if ((newEntry._overrideValue & osg::StateAttribute::ON) == 0) //TODO: check for higher override
      {
         // yes? remove it!
         accumShaderMap.erase(shaderID);
      }

      else
      {
         // see if there's a higher-up entry with the same ID:
         VirtualProgram::ShaderEntry& accumEntry = accumShaderMap[shaderID];

         // make sure we can add the new one:
         if ((accumEntry._shader.get() == 0L) ||                                   // empty slot, fill it
            ((accumEntry._overrideValue & osg::StateAttribute::PROTECTED) != 0) || // new entry is protected
            ((accumEntry._overrideValue & osg::StateAttribute::OVERRIDE) == 0))   // old entry does NOT override
         {
            accumEntry = newEntry;
         }
      }
   }

   /**
   * Apply the data binding information from a template program to the
   * target program.
   */
   void addTemplateDataToProgram(const osg::Program* templateProgram, osg::Program* program)
   {
      const osg::Program::FragDataBindingList& fbl = templateProgram->getFragDataBindingList();
      for (osg::Program::FragDataBindingList::const_iterator i = fbl.begin(); i != fbl.end(); ++i)
         program->addBindFragDataLocation(i->first, i->second);

      const osg::Program::UniformBlockBindingList& ubl = templateProgram->getUniformBlockBindingList();
      for (osg::Program::UniformBlockBindingList::const_iterator i = ubl.begin(); i != ubl.end(); ++i)
         program->addBindUniformBlock(i->first, i->second);
   }


   /**
   * Populates the specified Program with passed-in shaders.
   */
   void addShadersToProgram(const VirtualProgram::ShaderVector&      shaders,
      const VirtualProgram::AttribBindingList& attribBindings,
      const VirtualProgram::AttribAliasMap&    attribAliases,
      osg::Program*                            program)
   {
#ifdef USE_ATTRIB_ALIASES
      // apply any vertex attribute aliases. But first, sort them from longest to shortest 
      // so we don't get any overlap and bad replacements.
      VirtualProgram::AttribAliasVector sortedAliases;
      sortedAliases.reserve(attribAliases.size());
      sortedAliases.insert(sortedAliases.begin(), attribAliases.begin(), attribAliases.end());
      std::sort(sortedAliases.begin(), sortedAliases.end(), s_attribAliasSortFunc);

      for (VirtualProgram::ShaderVector::const_iterator i = shaders.begin(); i != shaders.end(); ++i)
      {
         osg::Shader* shader = i->get();
         applyAttributeAliases( shader, sortedAliases );
      }
#endif

      // merge the shaders if necessary.
      if ( s_mergeShaders )
      {
         unsigned          vertVersion = 0;
         HeaderMap         vertHeaders;
         std::stringstream vertBody;

         unsigned          fragVersion = 0;
         HeaderMap         fragHeaders;
         std::stringstream fragBody;

         // parse the shaders, combining header lines and finding the highest version:
         for (VirtualProgram::ShaderVector::const_iterator i = shaders.begin(); i != shaders.end(); ++i)
         {
            osg::Shader* s = i->get();
            if (s->getType() == osg::Shader::VERTEX)
            {
               parseShaderForMerging(s->getShaderSource(), vertVersion, vertHeaders, vertBody);
            }
            else if (s->getType() == osg::Shader::FRAGMENT)
            {
               parseShaderForMerging(s->getShaderSource(), fragVersion, fragHeaders, fragBody);
            }
         }

         // write out the merged shader code:
         std::string vertBodyText;
         vertBodyText = vertBody.str();
         std::stringstream vertShaderBuf;
         if (vertVersion > 0)
            vertShaderBuf << "#version " << vertVersion << "\n";
         for (HeaderMap::const_iterator h = vertHeaders.begin(); h != vertHeaders.end(); ++h)
            vertShaderBuf << h->second << "\n";
         vertShaderBuf << vertBodyText << "\n";
         vertBodyText = vertShaderBuf.str();

         std::string fragBodyText;
         fragBodyText = fragBody.str();
         std::stringstream fragShaderBuf;
         if (fragVersion > 0)
            fragShaderBuf << "#version " << fragVersion << "\n";
         for (HeaderMap::const_iterator h = fragHeaders.begin(); h != fragHeaders.end(); ++h)
            fragShaderBuf << h->second << "\n";
         fragShaderBuf << fragBodyText << "\n";
         fragBodyText = fragShaderBuf.str();

         // add them to the program.
         program->addShader(new osg::Shader(osg::Shader::VERTEX, vertBodyText));
         program->addShader(new osg::Shader(osg::Shader::FRAGMENT, fragBodyText));

         if (s_dumpShaders)
         {
            OE_NOTICE << LC
               << "\nMERGED VERTEX SHADER: \n\n" << vertBodyText << "\n\n"
               << "MERGED FRAGMENT SHADER: \n\n" << fragBodyText << "\n" << std::endl;
         }
      }
      else
      {
         for (VirtualProgram::ShaderVector::const_iterator i = shaders.begin(); i != shaders.end(); ++i)
         {
            program->addShader(i->get());
            if (s_dumpShaders)
            {
               OE_NOTIFY(osg::NOTICE, "")
                  << "----------\n"
                  << i->get()->getShaderSource() << std::endl;
            }
         }
      }

      // add the attribute bindings
      for (VirtualProgram::AttribBindingList::const_iterator abl = attribBindings.begin(); abl != attribBindings.end(); ++abl)
      {
         program->addBindAttribLocation(abl->first, abl->second);
      }
   }


   /**
   * Assemble a new OSG shader Program from the provided components.
   * Outputs the uniquely-identifying "key vector" and returns the new program.
   */
   osg::Program* buildProgram(const std::string&                  programName,
      osg::State&                         state,
      ShaderComp::FunctionLocationMap&    accumFunctions,
      VirtualProgram::ShaderMap&          accumShaderMap,
      VirtualProgram::AttribBindingList&  accumAttribBindings,
      VirtualProgram::AttribAliasMap&     accumAttribAliases,
      osg::Program*                       templateProgram,
      VirtualProgram::ShaderVector&       outputKeyVector)
   {

#ifdef DEBUG_ACCUMULATION

      // test dump .. function map and shader map should always include identical data.
      OE_INFO << LC << "====PROGRAM: " << programName << "\n";

      // debug:
      OE_INFO << LC << "====FUNCTIONS:\n";

      for (ShaderComp::FunctionLocationMap::iterator i = accumFunctions.begin();
         i != accumFunctions.end();
         ++i)
      {
         ShaderComp::OrderedFunctionMap& ofm = i->second;
         for (ShaderComp::OrderedFunctionMap::iterator j = ofm.begin(); j != ofm.end(); ++j)
         {
            OE_INFO << LC
               << j->second._name << ", " << (j->second.accept(state) ? "accepted" : "rejected")
               << std::endl;
         }
      }

      OE_INFO << LC << "====SHADERS:\n";
      for(VirtualProgram::ShaderMap::iterator i = accumShaderMap.begin();
         i != accumShaderMap.end();
         ++i)
      {
         OE_INFO << LC
            << i->first << ", "
            << (i->second.accept(state)?"accepted":"rejected") << ", "
            << i->second._overrideValue
            << std::endl;
      }
      OE_INFO << LC << "\n\n";
#endif

      // create new MAINs for this function stack.
      osg::Shader* vertMain = Registry::shaderFactory()->createVertexShaderMain( accumFunctions );
      osg::Shader* fragMain = Registry::shaderFactory()->createFragmentShaderMain( accumFunctions );

      // build a new "key vector" now that we've changed the shader map.
      // we call is a key vector because it uniquely identifies this shader program
      // based on its accumlated function set.
      for( VirtualProgram::ShaderMap::iterator i = accumShaderMap.begin(); i != accumShaderMap.end(); ++i )
      {
         outputKeyVector.push_back( i->data()._shader.get() );
      }

      // finally, add the mains (AFTER building the key vector .. we don't want or
      // need to mains in the key vector since they are completely derived from the
      // other elements of the key vector.)
      VirtualProgram::ShaderVector buildVector( outputKeyVector );
      buildVector.push_back( vertMain );
      buildVector.push_back( fragMain );

      if ( s_dumpShaders )
      {
         OE_NOTICE << LC << "\nPROGRAM: " << programName << " =============================\n" << std::endl;
      }

      // Create the new program.
      osg::Program* program = new osg::Program();
      program->setName(programName);
      addShadersToProgram(buildVector, accumAttribBindings, accumAttribAliases, program);
      addTemplateDataToProgram(templateProgram, program);

      return program;
   }
}

//------------------------------------------------------------------------

void
VirtualProgram::AttrStackMemory::remember(const osg::State&                 state,
const VirtualProgram::AttrStack&  rhs,
osg::Program*                     program)
{
   if (state.getFrameStamp())
   {
      unsigned contextID = state.getContextID();
      Item& item = _item[contextID];
      item.frameNumber = state.getFrameStamp()->getFrameNumber();
      item.attrStack = rhs;
      item.program = program;
   }
}

osg::Program*
VirtualProgram::AttrStackMemory::recall(const osg::State&                 state,
const VirtualProgram::AttrStack&  rhs)
{
   osg::Program* program = 0L;

   const osg::FrameStamp* fs = state.getFrameStamp();
   if (fs)
   {
      unsigned contextID = state.getContextID();
      Item& item = _item[contextID];

      // check frame number:
      if (item.program.valid() && item.frameNumber == fs->getFrameNumber())
      {
         // same, how compare the attribute stacks:
         if (item.attrStack.size() == rhs.size())
         {
            bool arraysMatch = true;
            for (unsigned int i = 0; i<item.attrStack.size() && arraysMatch; ++i)
            {
               if (item.attrStack[i] != rhs[i])
               {
                  arraysMatch = false;
               }
            }

            if (arraysMatch)
            {
               program = item.program.get();
            }
         }
      }

      if (!program)
      {
         item.frameNumber = fs->getFrameNumber();
         item.attrStack = rhs;
         item.program = 0L;
      }
   }

   return program;
}

//------------------------------------------------------------------------

VirtualProgram::ShaderEntry::ShaderEntry() :
_overrideValue(0)
{
   //nop
}

bool
VirtualProgram::ShaderEntry::accept(const osg::State& state) const
{
   return (!_accept.valid()) || (_accept->operator()(state) == true);
}

bool
VirtualProgram::ShaderEntry::operator < (const VirtualProgram::ShaderEntry& rhs) const
{
   if (_shader->compare(*rhs._shader.get()) < 0) return true;
   if (_overrideValue < rhs._overrideValue) return true;
   if (_accept.valid() && !rhs._accept.valid()) return true;
   return false;
}

//------------------------------------------------------------------------


namespace osgEarth
{
   class VirtualProgramSh : public VirtualProgram
   {
   public:
      VirtualProgramSh(const VirtualProgram& vp)
         : VirtualProgram(vp)
      {}

      virtual void
         apply(osg::State& state) const
      {

         if (_active.isSetTo(false))
         {
            return;
         }
         else if (!_active.isSet())
         {
            _active = Registry::capabilities().supportsGLSL();
         }

         const unsigned contextID = state.getContextID();

         if (_shaderMap.empty() && !_inheritSet)
         {
            // If there's no data in the VP, and never has been, unload any existing program.
            // NOTE: OSG's State processor creates a "global default attribute" for each type.
            // Sine we have no way of knowing whether the user created the VP or OSG created it
            // as the default fallback, we use the "_inheritSet" flag to differeniate. This
            // prevents any shader leakage from a VP-enabled node.

            // The following "if" helps performance a bit (based on profiler results) but a user
            // reported state corruption in the OSG stats display. The underlying cause is likely
            // in external code, but leave it commented out for now -gw 20150721

            //if ( state.getLastAppliedProgramObject() != 0L )
            {
               const osg::GL2Extensions* extensions = osg::GL2Extensions::Get(contextID, true);
               extensions->glUseProgram(0);
               state.setLastAppliedProgramObject(0);
            }
            return;
         }

         osg::ref_ptr<osg::Program> program;

         // Negate osg::State's last-attribute-applied tracking for 
         // VirtualProgram, since it cannot detect a VP that is reached from
         // different node/attribute paths. We replace this with the 
         // stack-memory construct below which will "remember" whether 
         // the VP has already been applied during the current frame using
         // the same an identical attribute stack.
         state.haveAppliedAttribute(this->SA_TYPE);

#ifdef USE_STACK_MEMORY
         bool programRecalled = false;
         const AttrStack* stack = StateEx::getProgramStack(state);
         if (stack)
         {
            program = _vpStackMemory.recall(state, *stack);
            programRecalled = program.valid();
         }
#endif // USE_STACK_MEMORY

         // We need to tracks whether there are any accept callbacks, because if so
         // we cannot store the program in stack memory -- the accept callback can
         // exclude shaders based on any condition.
         bool acceptCallbacksVary = _acceptCallbacksVaryPerFrame;

         if (!program.valid())
         {
            // Access the resuable shader map for this context. Bypasses reallocation overhead.
            ApplyVars& local = _apply[contextID];

            local.accumShaderMap.clear();
            local.accumAttribBindings.clear();
            local.accumAttribAliases.clear();
            local.keyVector.clear();

            // If we are inheriting, build the active shader map up to this point
            // (but not including this VP).
            if (_inherit)
            {
               accumulateShaders(
                  state,
                  _mask,
                  local.accumShaderMap,
                  local.accumAttribBindings,
                  local.accumAttribAliases,
                  acceptCallbacksVary);
            }

            // Next, add the shaders from this VP.
            {
               //Threading::ScopedReadLock readonly(_dataModelMutex);
               _dataModelMutex.lock();

               for (ShaderMap::const_iterator i = _shaderMap.begin(); i != _shaderMap.end(); ++i)
               {
                  if (i->data().accept(state))
                  {
                     addToAccumulatedMap(local.accumShaderMap, i->key(), i->data());
                  }
               }

               const AttribBindingList& abl = this->getAttribBindingList();
               local.accumAttribBindings.insert(abl.begin(), abl.end());

#ifdef USE_ATTRIB_ALIASES
               const AttribAliasMap& aliases = this->getAttribAliases();
               local.accumAttribAliases.insert(aliases.begin(), aliases.end());
#endif

               _dataModelMutex.unlock();
            }

            // next, assemble a list of the shaders in the map so we can use it as our
            // program cache key.
            // (Note: at present, the "cache key" does not include any information on the vertex
            // attribute bindings. Technically it should, but in practice this might not be an
            // issue; it is unlikely one would have two identical shader programs with different
            // bindings.)
            for (ShaderMap::iterator i = local.accumShaderMap.begin(); i != local.accumShaderMap.end(); ++i)
            {
               local.keyVector.push_back(i->data()._shader.get());
            }

            // current frame number, for shader program expiry.
            unsigned frameNumber = state.getFrameStamp() ? state.getFrameStamp()->getFrameNumber() : 0;

            // look up the program:
            {
               _programCacheMutex.lock();
               const_cast<VirtualProgramSh*>(this)->readProgramCache(local.keyVector, frameNumber, program);
               _programCacheMutex.unlock();
            }

            // if not found, lock and build it:
            if (!program.valid())
            {
               // build a new set of accumulated functions, to support the creation of main()
               ShaderComp::FunctionLocationMap accumFunctions;
               accumulateFunctions(state, accumFunctions);

               // now double-check the program cache, and failing that, build the
               // new shader Program.
               {
                  Threading::ScopedMutexLock lock(_programCacheMutex);

                  // double-check: look again to negate race conditions
                  const_cast<VirtualProgramSh*>(this)->readProgramCache(local.keyVector, frameNumber, program);
                  if (!program.valid())
                  {
                     local.keyVector.clear();

                     //OE_NOTICE << LC << "Building new Program for VP " << getName() << std::endl;

                     program = buildProgram(
                        "ShaderFix."+getName(),
                        state,
                        accumFunctions,
                        local.accumShaderMap,
                        local.accumAttribBindings,
                        local.accumAttribAliases,
                        _template.get(),
                        local.keyVector);

                     ///////////////////////////////////////////////////////////////////////////////////////////
                     /// Fix code starts here
                     fixProgram(program);
                     /// Fix code ends here
                     ///////////////////////////////////////////////////////////////////////////////////////////

                     if (_logShaders && program.valid())
                     {
                        std::stringstream buf;
                        for (unsigned i = 0; i < program->getNumShaders(); i++)
                        {
                           buf << program->getShader(i)->getShaderSource() << std::endl << std::endl;
                        }

                        if (_logPath.length() > 0)
                        {
                           std::fstream outStream;
                           outStream.open(_logPath.c_str(), std::ios::out);
                           if (outStream.fail())
                           {
                              OE_WARN << LC << "Unable to open " << _logPath << " for logging shaders." << std::endl;
                           }
                           else
                           {
                              outStream << buf.str();
                              outStream.close();
                           }
                        }
                        else
                        {
                           OE_NOTICE << LC << "Shader source: " << getName() << std::endl << "===============" << std::endl << buf.str() << std::endl << "===============" << std::endl;
                        }
                     }

                     // global sharing.
                     Registry::programSharedRepo()->share(program);

                     // finally, put own new program in the cache.
                     ProgramEntry& pe = _programCache[local.keyVector];
                     pe._program = program.get();
                     pe._frameLastUsed = frameNumber;

                     // purge expired programs.
                     const_cast<VirtualProgramSh*>(this)->removeExpiredProgramsFromCache(state, frameNumber);
                  }
               }
            }
         }

         // finally, apply the program attribute.
         if (program.valid())
         {
#ifdef USE_STACK_MEMORY
            // remember this program selection in case this VP is applied again
            // during the same frame.
            if (programRecalled == false &&   // recalled a program? not necessary
               getDataVariance() != DYNAMIC    &&   // DYNAMIC variance? might change during ST cull; no memory
               !acceptCallbacksVary            &&   // accept callbacks vary per frame? cannot use memory
               stack != 0L)
            {
               _vpStackMemory.remember(state, *stack, program.get());
            }
#endif // USE_STACK_MEMORY

            osg::Program::PerContextProgram* pcp;

#if OSG_VERSION_GREATER_OR_EQUAL(3,3,4)
            pcp = program->getPCP(state);
#else
            pcp = program->getPCP(contextID);
#endif
            bool useProgram = state.getLastAppliedProgramObject() != pcp;

#ifdef DEBUG_APPLY_COUNTS
            if (state.getFrameStamp() && state.getFrameStamp()->getFrameNumber() % 60 == 0)
            {
               // debugging            
               static int s_framenum = 0;
               static Threading::Mutex s_mutex;
               static std::map< const VirtualProgram*, std::pair<int, int> > s_counts;

               Threading::ScopedMutexLock lock(s_mutex);

               int framenum = state.getFrameStamp()->getFrameNumber();
               if (framenum > s_framenum)
               {
                  OE_NOTICE << LC << "Use program in last frame: " << std::endl;
                  for (std::map<const VirtualProgram*, std::pair<int, int> >::iterator i = s_counts.begin(); i != s_counts.end(); ++i)
                  {
                     std::pair<int, int>& counts = i->second;
                     OE_NOTICE << LC << "  "
                        << i->first->getName() << " : attemped=" << counts.first << ", applied=" << counts.second << "\n";
                  }
                  s_framenum = framenum;
                  s_counts.clear();
               }
               s_counts[this].first++;
               if (useProgram)
                  s_counts[this].second++;
            }
#endif

            if (useProgram)
            {
               if (pcp->needsLink())
                  program->compileGLObjects(state);

               if (pcp->isLinked())
               {
                  if (osg::isNotifyEnabled(osg::INFO))
                     pcp->validateProgram();

                  pcp->useProgram();
                  state.setLastAppliedProgramObject(pcp);
               }
               else
               {
                  // program not usable, fallback to fixed function.
                  const osg::GL2Extensions* extensions = osg::GL2Extensions::Get(contextID, true);
                  extensions->glUseProgram(0);
                  state.setLastAppliedProgramObject(0);
                  OE_DEBUG << LC << "Program link failure!" << std::endl;
               }
            }

            //program->apply( state );

#if 0 // test code for detecting race conditions
            for (int i = 0; i<10000; ++i) {
               state.setLastAppliedProgramObject(0L);
               program->apply(state);
            }
#endif
         }
      }

      void getShaderMap(ShaderMap& out) const
      {
         osgEarth::VirtualProgram::getShaderMap(out);
      }

   };

}

namespace ShaderFix
{

#define SAYBOOL(X) (X?"yes":"no")

   bool isSetGL2Support = false;

   void setGL2Support()
   {
      auto *cs = const_cast<osgEarth::Capabilities*>(&osgEarth::Registry::capabilities());
      static_assert(sizeof(osgEarth::Capabilities) == sizeof(osgEarth::CapabilitiesSh), "Update the osgEarth::CapabilitiesSh (hacking) class!");
      auto *rcs = reinterpret_cast<osgEarth::CapabilitiesSh*>(cs);
      rcs->_supportsGLSL = true;
      rcs->_maxGPUTextureUnits = 8;
	   rcs->_maxFastTextureSize = rcs->_maxTextureSize = 8192;

      // TRY CORRECTLY GET CAPABILITIES
      auto *gc = QOpenGLContext::currentContext();
      if (gc)
      {
         auto *fs = gc->functions();
         if (fs)
         {
            auto *gstr = fs->glGetString(GL_VENDOR);
            auto *rndr = fs->glGetString(GL_RENDERER);
            if (gstr && rndr && fs->glGetString(GL_VERSION))
            {
               isSetGL2Support = true;
#ifdef WIN32
               if (strstr((char const*)rndr, "Intel") != 0)
               {
                  _putenv_s("OSG_TEXT_INCREMENTAL_SUBLOADING", "OFF");  // use overwork-patch in OSG Glyph.cpp to load text glyphs
               }
#endif

#ifndef GL_MAX_TEXTURE_IMAGE_UNITS_ARB
#define GL_MAX_TEXTURE_IMAGE_UNITS_ARB    0x8872
               OE_WARN << "Warn: GL_MAX_TEXTURE_IMAGE_UNITS_ARB was not defined, defaulting to 0x8872\n";
#endif
               fs->glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS_ARB, &rcs->_maxGPUTextureUnits);
               OE_WARN << "  Max GPU texture units = " << rcs->_maxGPUTextureUnits << std::endl;
               fs->glGetIntegerv(GL_MAX_TEXTURE_SIZE, &rcs->_maxTextureSize);
               OE_WARN << "  Max texture size = " << rcs->_maxTextureSize << std::endl;
               rcs->_maxFastTextureSize = rcs->_maxTextureSize;
            }
         }
      }

   }


   // these fixes are mainly for Wandboard
   osg::Node* createShaderCompositionScene(const char*fname)
   {
      osg::Node *node = osgDB::readNodeFile(fname);
      if (!node) return 0;

      auto *grp = dynamic_cast<osg::Group*>(node);

      while (grp && grp->getNumChildren() == 1)
      {
         auto *ifgrp = dynamic_cast<osg::Group*>(grp->getChild(0));
         auto *ifgeode = dynamic_cast<osg::Geode*>(ifgrp);
         if (ifgrp && !ifgeode)
            grp = ifgrp;
         else break;
      }

      if (grp == nullptr || grp->getNumChildren() == 0)
         return 0;

      for (unsigned int chil = 0; chil < grp->getNumChildren(); chil++)
      {
         auto *geode = dynamic_cast<osg::Geode*>(grp->getChild(chil));
         auto *geomet = dynamic_cast<osg::Geometry*>(geode->getChild(0));

         auto *verts = dynamic_cast<osg::Vec3Array*>(geomet->getVertexArray());
         geomet->setVertexAttribArray(6, verts, osg::Array::BIND_PER_VERTEX);


         auto *mat = dynamic_cast<const osg::Material*>(geomet->getStateSet()->getAttribute(osg::StateAttribute::Type::MATERIAL));
         auto clr = mat->getDiffuse(osg::Material::Face::FRONT);
         osg::Vec4Array* colors = new osg::Vec4Array;
         colors->assign(verts->getNumElements(), clr);

         geomet->setVertexAttribArray(7, colors, osg::Array::BIND_PER_VERTEX);
      }

      for (unsigned int chil = 0; chil < grp->getNumChildren(); chil++)
      {
         auto *geode = dynamic_cast<osg::Geode*>(grp->getChild(chil));
         auto *geomet = dynamic_cast<osg::Geometry*>(geode->getChild(0));
         geomet->getStateSet()->removeAttribute(osg::StateAttribute::Type::MATERIAL);
      }

      osg::ref_ptr<osg::StateSet> stateset = grp->getOrCreateStateSet();
      osg::ref_ptr<osg::Program> program = new osg::Program;
      stateset->setAttributeAndModes(program.get(), osg::StateAttribute::ON);


      // declare shaders
      char vertSource[] =
         "attribute vec4 a_col;\n"
         "varying vec4 v_col;"

         "void main(void)\n"
         "{\n"
         "gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
         "v_col = a_col;\n"
         "}\n";

      char fragSource[] =
         "varying vec4 v_col;\n"
         "void main(void)\n"
         "{\n"
         "gl_FragColor = v_col;\n"
         "}\n";

      // set shader
      program->setName("simple shader");
      program->addShader(new osg::Shader(osg::Shader::VERTEX, vertSource));
      program->addShader(new osg::Shader(osg::Shader::FRAGMENT, fragSource));
      program->addBindAttribLocation("a_col", 7);

      return node;
   }

   // this fix was for wandboard, with some special aero.earth options
   void fixEarth(osgEarth::MapNode *earth)
   {
      std::map<osgEarth::VirtualProgram*, osgEarth::VirtualProgramSh*> vpMap;
      std::set<osg::Program*> pSet;

      auto checkNodeShaders = [&vpMap,&pSet](osg::Node *ch)
      {
         auto *dr = dynamic_cast<osg::Drawable*>(ch);
         if (dr)
         {
            auto *geom = dynamic_cast<osg::Geometry*>(ch);
            if (geom)
            {
                   auto *verts = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());
                  if (verts)
                     geom->setVertexAttribArray(BIND_VERTEX, verts, osg::Array::BIND_PER_VERTEX);
                  auto *norms = dynamic_cast<osg::Vec3Array*>(geom->getNormalArray());
                 if (norms)
                    geom->setVertexAttribArray(BIND_NORMAL, norms, osg::Array::BIND_PER_VERTEX);
            }
         }

         auto *chss = ch->getStateSet();
         if (chss)
         {
            auto *pa = chss->getAttribute(osg::StateAttribute::Type::PROGRAM);
            if (pa)
            {
               auto *pp = dynamic_cast<osg::Program*>(pa);
               if (pp)
               {
                   pSet.insert(pp);
                  fixProgram(pp);
               }
               else
               {
                  auto *vp = dynamic_cast<osgEarth::VirtualProgram*>(pa);
                  auto *vpsh = dynamic_cast<osgEarth::VirtualProgramSh*>(pa);
                  if (vp && vpsh == nullptr)
                  {
                     auto fvp = vpMap.find(vp);
                     if (fvp == vpMap.end())
                     {
                        vpMap[vp] = new osgEarth::VirtualProgramSh(*vp);
                     }
                     chss->setAttribute(vpMap[vp]);
                  }
               }
            }

         }

      };

      std::function<void(osg::Group*)> goNodeChilds;
      goNodeChilds = [&goNodeChilds, &checkNodeShaders](osg::Group* grp)
      {
         for (unsigned int chil = 0; chil < grp->getNumChildren(); chil++)
         {
            auto *ch = grp->getChild(chil);

            checkNodeShaders(ch);

            auto *gr2 = dynamic_cast<osg::Group*>(ch);
            if (gr2)
               goNodeChilds(gr2);
         }
      };

      checkNodeShaders(earth);
      goNodeChilds(earth);

      for (auto &vp : vpMap)
      {
         VirtualProgramSh *vsh = reinterpret_cast<VirtualProgramSh*>(vp.second);   // Un-Public
         osgEarth::VirtualProgram::ShaderMap sm;
         vsh->getShaderMap(sm);

         for (auto &sme : sm)
         {
            osg::Shader *sh = sme.data()._shader;
            if (sh)
            {
               fixShader(sh);
            }
         }
      }

   }
      
};
