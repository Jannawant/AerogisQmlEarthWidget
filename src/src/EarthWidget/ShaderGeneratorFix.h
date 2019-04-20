
/*
*
*   based on osgEarth/ShaderGenerator
*
*/



#ifndef OSGEARTH_SHADER_GENERATOR_FIX_H
#define OSGEARTH_SHADER_GENERATOR_FIX_H 1

#include <osgEarth/ShaderGenerator>


// forward declarations
namespace osg
{
	class TexEnv;
	class TexGen;
	class TexMat;
	class Texture1D;
	class Texture2D;
	class Texture3D;
	class TextureRectangle;
	class Texture2DArray;
	class Texture2DMultisample;
	class TextureCubeMap;
	class PointSprite;
}

namespace osgSim
{
	class LightPointNode;
}

namespace osgEarth
{
	/**
	* Traverses a scene graph and generates VirtualProgram attributes to
	* render the geometry using GLSL shaders.
	*
	* You can use this class directly, but they osgEarth Registry holds
	* a system-wide implementation that the user can replace. So the best
	* way to use this class is:
	*
	*   osgEarth::Registry::shaderGenerator().run(graph);
	*
	* After generating shaders, the scene graph will have MANY additional
	* StateSets. For performance reasons you should run a StateSet sharing
	* pass afterwards. You can do this by running the StateSetCache
	* optimization function:
	*
	*   osgEarth::StateSetCache::optimize(graph)
	*
	* Or you can pass a StateSetCache instance into the ShaderGenerator::run()
	* method and it will perform state sharing internally.
	*
	* Implementation Notes:
	*
	* ShaderGenerator WILL NOT modify existing StateSets. Instead, when
	* a state change is necessary (to inject uniforms or virtual programs)
	* it will clone the existing StateSet and replace it with a
	* modified version. We do this to avoid altering StateSets that might
	* be shared or in the live scene graph.
	*/
	class ShaderGeneratorFix : public ShaderGenerator
	{
	public:
		/** Constructs a new shader generator */
		ShaderGeneratorFix();

		/** Copy constructor */
		ShaderGeneratorFix(const ShaderGenerator& rhs, const osg::CopyOp& copy);

	protected: // high-level entry points:

		virtual bool processGeometry(const osg::StateSet* stateSet, osg::ref_ptr<osg::StateSet>& replacement);

	};

	struct ShaderGeneratorProxyFake //header only
	{
		osg::ref_ptr<ShaderGenerator> _instance;
	};


}

#endif // OSGEARTH_SHADER_GENERATOR_FIX_H
