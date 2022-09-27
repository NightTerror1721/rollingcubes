#include "window.h"

#include <iostream>

namespace window
{
	static GLFWwindow* mainw = nullptr;
}

namespace window
{
	GLFWwindow* getMainWindow() { return mainw; }

	bool createMainWindow()
	{
		if (mainw == nullptr)
		{
			gl::initGLFW();

			mainw = glfwCreateWindow(default_width, default_height, "Rollingcube", nullptr, nullptr);
			if (mainw == nullptr)
			{
				fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible.\n");
				glfwTerminate();
				return false;
			}
			glfwMakeContextCurrent(mainw);

			gl::initGLEW();

			glfwSetInputMode(mainw, GLFW_STICKY_KEYS, GL_TRUE);

			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
			glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
		}

		return true;
	}

	void simpleLoop(bool terminateOnEnd, const std::function<void(void)>& drawFunction)
	{
		do
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			drawFunction();

			glfwSwapBuffers(mainw);
			glfwPollEvents();
		} while (glfwGetKey(mainw, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(mainw));

		if (terminateOnEnd)
			gl::terminate();
	}
}
