#ifndef SHADERFIX_H
#define SHADERFIX_H

#include <osgEarth/MapNode>

namespace ShaderFix
{
   void setGL2Support();

   extern bool isSetGL2Support;

   osg::Node* createShaderCompositionScene(const char*fname);

   void fixEarth(osgEarth::MapNode *earth);
}

namespace osg
{
   class StateSh : public State
   {
   public:
      EnabledVertexAttribArrayList& getVertexAttribArrayList()
      {
         return _vertexAttribArrayList;
      }
   };
}

namespace osgEarth
{
   /**
   * Stores information about the hardware and graphics system capbilities.
   * The osgEarth::Registry stores a singleton Capabilities object that you can
   * use to determine what your system supports.
   */
   class OSGEARTH_EXPORT CapabilitiesSh : public osg::Referenced
   {
   public:
      CapabilitiesSh() {}

      /** dtor */
      virtual ~CapabilitiesSh() { }

      int  _maxFFPTextureUnits;
      int  _maxGPUTextureUnits;
      int  _maxGPUTextureCoordSets;
      int  _maxGPUAttribs;
      int  _maxTextureSize;
      int  _maxFastTextureSize;
      int  _maxLights;
      int  _depthBits;
      bool _supportsGLSL;
      float _GLSLversion;
      bool _supportsTextureArrays;
      bool _supportsTexture3D;
      bool _supportsMultiTexture;
      bool _supportsStencilWrap;
      bool _supportsTwoSidedStencil;
      bool _supportsTexture2DLod;
      bool _supportsMipmappedTextureUpdates;
      bool _supportsDepthPackedStencilBuffer;
      bool _supportsOcclusionQuery;
      bool _supportsDrawInstanced;
      bool _supportsUniformBufferObjects;
      bool _supportsNonPowerOfTwoTextures;
      int  _maxUniformBlockSize;
      bool _preferDLforStaticGeom;
      int  _numProcessors;
      bool _supportsFragDepthWrite;
      std::string _vendor;
      std::string _renderer;
      std::string _version;
      bool _supportsS3TC;
      bool _supportsPVRTC;
      bool _supportsARBTC;
      bool _supportsETC;
      bool _supportsRGTC;
      bool _isGLES;
      bool _supportsTextureBuffer;
      int  _maxTextureBufferSize;

      friend class Registry;
   };

}


#endif // SHADERFIX_H