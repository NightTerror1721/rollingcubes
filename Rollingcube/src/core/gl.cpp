#include "gl.h"

#include <iostream>

#include "engine/texture.h"
#include "engine/model.h"
#include "engine/shader.h"
#include "engine/lua/lua.h"
#include "game/theme.h"


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
		BlockTemplateManager::instance().clear();
		TileTemplateManager::instance().clear();
		ModelObjectTemplateManager::instance().clear();
		BallTemplateManager::instance().clear();
		Theme::releaseCurrentTheme();
		LuaScriptManager::instance().clear();

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
