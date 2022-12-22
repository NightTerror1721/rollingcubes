#include "gl.h"

#include <iostream>

#include "engine/texture.h"
#include "engine/model.h"
#include "engine/shader.h"
#include "engine/lua/module.h"
#include "game/theme.h"

#include "engine/lua/module.h"
#include "utils/lualib_constants.h"


namespace gl
{
	bool initGLFW()
	{
		if (!glfwInit())
		{
			const char* msg = new char[256];
			glfwGetError(&msg);
			std::cerr << "GLFW INITI ERROR: " << msg << std::endl;
			delete[] msg;
			return false;
		}

		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		return true;
	}

	bool initGLEW()
	{
		glewExperimental = true;
		GLenum result;
		if ((result = glewInit()) != GLEW_OK)
		{
			glfwTerminate();
			std::cerr << "GLEW INITIATING ERROR: " << glewGetErrorString(result) << std::endl;
			return false;
		}
		return true;
	}

	void terminate()
	{
		SkyboxTemplateManager::instance().clear();
		BlockTemplateManager::instance().clear();
		TileTemplateManager::instance().clear();
		ModelObjectTemplateManager::instance().clear();
		BallTemplateManager::instance().clear();
		Theme::releaseCurrentTheme();
		LuaModuleManager::instance().clear();

		TextureManager::root().clear();
		CubeMapTextureManager::root().clear();
		ModelManager::root().clear();
		ShaderManager::vertex().clear();
		ShaderManager::fragment().clear();
		ShaderManager::geometry().clear();
		ShaderProgramManager::instance().clear();

		glfwTerminate();
	}
}













namespace lua::lib
{
	namespace LUA_gl { static defineLuaLibraryConstructor(registerToLua, root, state); }

	void registerGlLibToLua()
	{
		LuaLibraryManager::instance().registerLibrary(
			::lua::lib::names::gl,
			&LUA_gl::registerToLua,
			{}
		);
	}
}

namespace lua::lib::LUA_gl
{
	namespace depthfun
	{
		using type = GLenum;

		static constexpr type Never = type(gl::DepthFunction::Never);
		static constexpr type Less = type(gl::DepthFunction::Less);
		static constexpr type Equal = type(gl::DepthFunction::Equal);
		static constexpr type LessOrEqual = type(gl::DepthFunction::LessOrEqual);
		static constexpr type Greater = type(gl::DepthFunction::Greater);
		static constexpr type NotEqual = type(gl::DepthFunction::NotEqual);
		static constexpr type GreaterOrEqual = type(gl::DepthFunction::GreaterOrEqual);
		static constexpr type Always = type(gl::DepthFunction::Always);
		static constexpr type Default = type(gl::DepthFunction::Default);
	}
	static void setDepthFunction(GLenum depthFunction) { gl::setDepthFunction(gl::DepthFunction(depthFunction)); }

	namespace cullface
	{
		using type = GLenum;

		static constexpr type Front = type(gl::CullFace::Front);
		static constexpr type Back = type(gl::CullFace::Back);
		static constexpr type FrontAndBack = type(gl::CullFace::FrontAndBack);
		static constexpr type Default = type(gl::CullFace::Default);
	}
	static void setCullFace(GLenum face) { gl::setCullFace(gl::CullFace(face)); }

	namespace frontface
	{
		using type = GLenum;

		static constexpr type Clockwise = type(gl::FrontFace::Clockwise);
		static constexpr type CounterClockwise = type(gl::FrontFace::CounterClockwise);
		static constexpr type Default = type(gl::FrontFace::Default);
	}
	static void setFrontFace(GLenum face) { gl::setFrontFace(gl::FrontFace(face)); }


	static defineLuaLibraryConstructor(registerToLua, root, state)
	{
		namespace meta = lua::metamethod;


		root = root.beginNamespace("GL")
				.beginNamespace("DepthFunction")
					.addVariable("Never", depthfun::Never)
					.addVariable("Less", depthfun::Less)
					.addVariable("Equal", depthfun::Equal)
					.addVariable("LessOrEqual", depthfun::LessOrEqual)
					.addVariable("Greater", depthfun::Greater)
					.addVariable("NotEqual", depthfun::NotEqual)
					.addVariable("GreaterOrEqual", depthfun::GreaterOrEqual)
					.addVariable("Always", depthfun::Always)
					.addVariable("Default", depthfun::Default)
				.endNamespace()
				.beginNamespace("CullFace")
					.addVariable("Front", cullface::Front)
					.addVariable("Back", cullface::Back)
					.addVariable("FrontAndBack", cullface::FrontAndBack)
					.addVariable("Default", cullface::Default)
				.endNamespace()
				.beginNamespace("FrontFace")
					.addVariable("Clockwise", frontface::Clockwise)
					.addVariable("CounterClockwise", frontface::CounterClockwise)
					.addVariable("Default", frontface::Default)
				.endNamespace()

				.addFunction("setDepthFunction", &setDepthFunction)
				.addFunction("enableDepthTest", &gl::enableDepthTest)
				.addFunction("disableDepthTest", &gl::disableDepthTest)
				.addFunction("setCullFace", &setCullFace)
				.addFunction("setFrontFace", &setFrontFace)
				.addFunction("enableCullFace", &gl::enableCullFace)
				.addFunction("disableCullFace", &gl::disableCullFace)
			.endNamespace();

		return true;
	}
}
