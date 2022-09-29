#pragma once

#pragma warning(push)
#pragma warning(disable: 26819)

//#include <SDL.h>
//#include <GL/glew.h>
//#include <SDL_opengl.h>
//#include <GL/GLU.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#pragma warning(pop)


namespace gl
{
	bool initGLFW();
	bool initGLEW();
	void terminate();
}
