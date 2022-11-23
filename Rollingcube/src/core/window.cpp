#include "window.h"

#include <iostream>
#include <sstream>


namespace window
{
	static GLFWwindow* mainw = nullptr;
}

namespace window
{
	GLFWwindow* getMainWindow() { return mainw; }

	Dimension getMainWindowSize()
	{
		Dimension dim;
		glfwGetWindowSize(mainw, &dim.width, &dim.height);
		return dim;
	}

	bool createMainWindow(const Dimension& windowSize)
	{
		if (mainw == nullptr)
		{
			gl::initGLFW();

			mainw = glfwCreateWindow(windowSize.width, windowSize.height, "Rollingcube", nullptr, nullptr);
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

			glEnable(GL_CULL_FACE);

			glfwSwapInterval(1);

			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		}

		return true;
	}

	void simpleLoop(bool terminateOnEnd, const std::function<void(Time)>& drawFunction, const std::function<void(const TimeController&)>& endDrawFunction)
	{
		TimeController timeController;
		do
		{
			Time elapsedTime = timeController.update();
			std::stringstream ss;
			ss << "Rollingcube " << timeController.getFPS() << "fps";
			glfwSetWindowTitle(getMainWindow(), ss.str().c_str());

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			drawFunction(elapsedTime);
			endDrawFunction(timeController);

			glfwSwapBuffers(mainw);
			glfwPollEvents();
		} while (glfwGetKey(mainw, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(mainw));

		if (terminateOnEnd)
			gl::terminate();
	}
}
