#pragma once

#include <functional>

#include "gl.h"
#include "time.h"


namespace window
{
	struct Dimension
	{
		int width;
		int height;
	};
}


namespace window
{
	inline constexpr std::size_t default_width = 1024;
	inline constexpr std::size_t default_height = 768;


	GLFWwindow* getMainWindow();

	Dimension getMainWindowSize();

	bool createMainWindow(const Dimension& windowSize);

	void simpleLoop(bool terminateOnEnd, const std::function<void(Time)>& drawFunction = {}, const std::function<void(const TimeController&)>& endDrawFunction = {});
}

namespace window
{
	inline int getMainWindowWidth() { return getMainWindowSize().width; }
	inline int getMainWindowHeight() { return getMainWindowSize().height; }
}
