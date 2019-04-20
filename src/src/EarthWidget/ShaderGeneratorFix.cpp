

/*
*
*   based on osgEarth/ShaderGenerator.cpp
*
*/




#include "ShaderGeneratorFix.h"
#include <osgEarth/ShaderGenerator>
#include <osgEarth/Capabilities>
#include <osgEarth/ImageUtils>
#include <osgEarth/Registry>
#include <osgEarth/ShaderFactory>
#include <osgEarth/StringUtils>
#include <osgEarth/URI>

#include <osg/Drawable>
#include <osg/Geode>
#include <osg/PagedLOD>
#include <osg/ProxyNode>
#include <osg/Texture1D>
#include <osg/Texture2D>
#include <osg/Texture3D>
#include <osg/TextureRectangle>
#include <osg/Texture2DMultisample>
#include <osg/Texture2DArray>
#include <osg/TextureBuffer>
#include <osg/TextureCubeMap>
#include <osg/TexEnv>
#include <osg/TexGen>
#include <osg/TexMat>
#include <osg/ClipNode>
#include <osg/PointSprite>
#include <osg/ValueObject>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osgDB/ReadFile>
#include <osgText/Text>
#include <osgSim/LightPointNode>


// compatibility string for GLES:

#ifdef OSG_GLES2_AVAILABLE
#   define GLSL_PRECISION "precision mediump float;"
#   define MEDIUMP        "mediump "
#   define LOWP           "lowp "
#   define HIGHP          "highp "
#else
#   define GLSL_PRECISION ""
#   define MEDIUMP        ""
#   define LOWP           ""
#   define HIGHP          ""
#endif

// shader names
#define TEX_COORD      "oe_sg_texcoord"
#define TEX_COORD_TEXT "oe_sg_texcoord_text"
#define SAMPLER        "oe_sg_sampler"
#define SAMPLER_TEXT   "oe_sg_sampler_text"
#define ATTRIB         "oe_sg_attrib"
#define TEXENV_COLOR   "oe_sg_texenvcolor"
#define TEX_MATRIX     "oe_sg_texmat"

#define VERTEX_FUNCTION   "oe_sg_vert"
#define FRAGMENT_FUNCTION "oe_sg_frag"

// other stuff
#define INDENT "    "

namespace osgEarth
{
	struct ActiveAttributeCollector : public osg::StateAttribute::ModeUsage
	{
		ActiveAttributeCollector(osg::StateSet* stateset, const osg::StateAttribute* sa, unsigned unit = 0) :
			_stateset(stateset),
			_sa(const_cast<osg::StateAttribute*>(sa)),
			_unit(unit) {}

		virtual ~ActiveAttributeCollector() {}

		virtual void usesMode(osg::StateAttribute::GLMode mode)
		{
			if (_stateset->getMode(mode) & osg::StateAttribute::ON)
			{
				if (_sa->isTextureAttribute())
					_stateset->setTextureAttribute(_unit, _sa, osg::StateAttribute::ON);
				else
					_stateset->setAttribute(_sa, osg::StateAttribute::ON);
			}
		}

		virtual void usesTextureMode(osg::StateAttribute::GLMode mode)
		{
			if (_stateset->getTextureMode(_unit, mode) & osg::StateAttribute::ON)
			{
				if (_sa->isTextureAttribute())
					_stateset->setTextureAttribute(_unit, _sa, osg::StateAttribute::ON);
				else
					_stateset->setAttribute(_sa, osg::StateAttribute::ON);
			}
		}

		osg::StateSet*       _stateset;
		osg::StateAttribute* _sa;
		unsigned             _unit;
	};


	/**
	* The OSG State extended with mode/attribute accessors.
	*/
	class StateEx : public osg::State
	{
	public:
		StateEx() : State() {}

		// Captures the ACTIVE state into a state set. i.e., only state attributes
		// set to ON.
		osg::StateSet* capture() const
		{
			osg::StateSet* stateset = new osg::StateSet();

			// add ON modes to the new stateset:
			for (ModeMap::const_iterator i = _modeMap.begin();
				i != _modeMap.end();
				++i)
			{
				// note GLMode = mitr->first
				const ModeStack& ms = i->second;
				if (!ms.valueVec.empty())
				{
					stateset->setMode(i->first, ms.valueVec.back());
				}
			}

			// add ON texture modes to the new stateset:
			for (unsigned unit = 0; unit<_textureModeMapList.size(); ++unit)
			{
				const ModeMap& modeMap = _textureModeMapList[unit];
				for (ModeMap::const_iterator i = modeMap.begin(); i != modeMap.end(); ++i)
				{
					const ModeStack& ms = i->second;
					if (!ms.valueVec.empty())
					{
						stateset->setTextureMode(unit, i->first, ms.valueVec.back());
					}
				}
			}

			for (AttributeMap::const_iterator i = _attributeMap.begin();
				i != _attributeMap.end();
				++i)
			{
				const AttributeStack& as = i->second;
				if (!as.attributeVec.empty())
				{
					const osg::State::AttributePair& pair = as.attributeVec.back();
					osg::StateAttribute* sa = const_cast<osg::StateAttribute*>(pair.first);
					ActiveAttributeCollector collector(stateset, sa);
					bool modeless = isModeless(sa) || !sa->getModeUsage(collector);
					if (modeless)
					{
						// if getModeUsage returns false, there are no modes associated with
						// this attr, so just add it (it can't be forcably disabled)
						stateset->setAttribute(sa, osg::StateAttribute::ON);
					}
				}
			}

			for (unsigned unit = 0; unit<_textureAttributeMapList.size(); ++unit)
			{
				const AttributeMap& attrMap = _textureAttributeMapList[unit];
				for (AttributeMap::const_iterator i = attrMap.begin(); i != attrMap.end(); ++i)
				{
					const AttributeStack& as = i->second;
					if (!as.attributeVec.empty())
					{
						const osg::State::AttributePair& pair = as.attributeVec.back();
						osg::StateAttribute* sa = const_cast<osg::StateAttribute*>(pair.first);
						ActiveAttributeCollector collector(stateset, sa, unit);
						bool modeless = isModeless(sa) || !sa->getModeUsage(collector);
						if (modeless)
						{
							// if getModeUsage returns false, there are no modes associated with
							// this attr, so just add it (it can't be forcably disabled)
							stateset->setTextureAttribute(unit, sa, osg::StateAttribute::ON);
						}
						else
						{
							///////////// THIS IS MAIN FIX
#ifndef OSG_GL_FIXED_FUNCTION_AVAILABLE
							stateset->setTextureAttribute(unit, sa, osg::StateAttribute::ON);
#endif
						}
					}
				}
			}

			return stateset;
		}

		// some attrs dont' properly report mode usage until OSG 3.3.1.
		// ref: https://github.com/openscenegraph/osg/commit/22af59482ac4f727eeed5b97476a3a47d7fe8a69
		bool isModeless(osg::StateAttribute* sa) const
		{
#if OSG_VERSION_LESS_THAN(3,3,1)            
			return
				dynamic_cast<osg::Texture2DArray*>(sa) ||
				dynamic_cast<osg::Texture2DMultisample*>(sa) ||
				dynamic_cast<osg::TextureBuffer*>(sa);
#else
			return false;
#endif
		}
	};




	/////////////////////ShaderGeneratorFix BEGIN//////////////////


	/** Constructs a new shader generator */
	ShaderGeneratorFix::ShaderGeneratorFix()
	{
		// just use parent
	}

	/** Copy constructor */
	ShaderGeneratorFix::ShaderGeneratorFix(const ShaderGenerator& rhs, const osg::CopyOp& copy)
		: ShaderGenerator(rhs, copy)
	{

	}


	bool
	ShaderGeneratorFix::processGeometry(const osg::StateSet* original,
		osg::ref_ptr<osg::StateSet>& replacement)
	{
		// do nothing if there's no GLSL support
		if (!_active)
			return false;

		// capture the active current state:
		osg::ref_ptr<osg::StateSet> current = static_cast<StateEx*>(_state.get())->capture();

		// check for a real osg::Program in the whole state stack. If it exists, bail out
		// so that OSG can use the program already in the graph. We never override a
		// full Program.
		osg::StateAttribute* program = current->getAttribute(osg::StateAttribute::PROGRAM);
		if (dynamic_cast<osg::Program*>(program) != 0L)
			return false;

		// Copy or create a new stateset (that we may or may not use depending on
		// what we find). Never modify an existing stateset!
		osg::ref_ptr<osg::StateSet> newStateSet =
			original ? osg::clone(original, osg::CopyOp::SHALLOW_COPY) :
			new osg::StateSet();

		// likewise, create a VP that we might populate.
		osg::ref_ptr<VirtualProgram> vp = VirtualProgram::cloneOrCreate(original, newStateSet);

		// we'll set this to true if the new stateset goes into effect and
		// needs to be returned.
		bool needNewStateSet = false;
		bool needVertexFunction = false;
		bool needFragmentFunction = false;

		// give the VP a name if it needs one.
		if (vp->getName().empty())
		{
			vp->setName(_name);
		}

		// Check whether the lighting state has changed and install a mode uniform.
		// TODO: fix this
		if (original && original->getMode(GL_LIGHTING) != osg::StateAttribute::INHERIT)
		{
			needNewStateSet = true;
			osg::StateAttribute::GLModeValue value = current->getMode(GL_LIGHTING);
			newStateSet->addUniform(Registry::shaderFactory()->createUniformForGLMode(GL_LIGHTING, value));
		}

		// start generating the shader source.
		GenBuffers buf;
		buf._stateSet = newStateSet.get();

		// if the stateset changes any texture attributes, we need a new virtual program:
		if (current->getTextureAttributeList().size() > 0)
		{
			bool wroteTexelDecl = false;

			// Loop over all possible texture image units.
			int maxUnit = Registry::capabilities().getMaxGPUTextureUnits();

			for (int unit = 0; unit < maxUnit; ++unit)
			{
				if (!wroteTexelDecl)
				{
					buf._fragBody << INDENT << MEDIUMP "vec4 texel; \n";
					wroteTexelDecl = true;
				}

				osg::Texture* tex = dynamic_cast<osg::Texture*>(current->getTextureAttribute(unit, osg::StateAttribute::TEXTURE));

				if (accept(tex) && !ImageUtils::isFloatingPointInternalFormat(tex->getInternalFormat()))
				{
					osg::TexGen* texgen = dynamic_cast<osg::TexGen*>(current->getTextureAttribute(unit, osg::StateAttribute::TEXGEN));
					osg::TexEnv* texenv = dynamic_cast<osg::TexEnv*>(current->getTextureAttribute(unit, osg::StateAttribute::TEXENV));
					osg::TexMat* texmat = dynamic_cast<osg::TexMat*>(current->getTextureAttribute(unit, osg::StateAttribute::TEXMAT));
					osg::PointSprite* sprite = dynamic_cast<osg::PointSprite*>(current->getTextureAttribute(unit, osg::StateAttribute::POINTSPRITE));

					if (apply(tex, texgen, texenv, texmat, sprite, unit, buf) == true)
					{
						needNewStateSet = true;
					}
				}
			}
		}

		// Process the state attributes.
		osg::StateSet::AttributeList& attrs = current->getAttributeList();
		if (apply(attrs, buf))
		{
			needNewStateSet = true;
		}

		if (needNewStateSet)
		{
			std::string version = Stringify() << buf._version;

			std::string vertHeadSource;
			vertHeadSource = buf._vertHead.str();

			std::string vertBodySource;
			vertBodySource = buf._vertBody.str();


			if (!vertHeadSource.empty() || !vertBodySource.empty())
			{
				std::string vertSource = Stringify()
					<< "#version " << version << "\n" GLSL_PRECISION "\n"
					<< vertHeadSource
					<< "void " VERTEX_FUNCTION "(inout vec4 vertex_view)\n{\n"
					<< vertBodySource
					<< "}\n";

				vp->setFunction(VERTEX_FUNCTION, vertSource, ShaderComp::LOCATION_VERTEX_VIEW, 0.5f);
			}


			std::string fragHeadSource;
			fragHeadSource = buf._fragHead.str();

			std::string fragBodySource;
			fragBodySource = buf._fragBody.str();

			if (!fragHeadSource.empty() || !fragBodySource.empty())
			{
				std::string fragSource = Stringify()
					<< "#version " << version << "\n" GLSL_PRECISION "\n"
					<< fragHeadSource
					<< "void " FRAGMENT_FUNCTION "(inout vec4 color)\n{\n"
					<< fragBodySource
					<< "}\n";

				vp->setFunction(FRAGMENT_FUNCTION, fragSource, ShaderComp::LOCATION_FRAGMENT_COLORING, 0.5f);
			}
		}

		if (needNewStateSet)
		{
			replacement = newStateSet.get();
		}
		return replacement.valid();
	}

	/////////////////////ShaderGeneratorFix END//////////////////
}