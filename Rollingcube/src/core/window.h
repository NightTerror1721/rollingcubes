#pragma once

#include <functional>

#include "gl.h"
#include "time.h"

namespace window
{
	inline constexpr std::size_t default_width = 1024;
	inline constexpr std::size_t default_height = 768;


	GLFWwindow* getMainWindow();

	bool createMainWindow();

	void simpleLoop(bool terminateOnEnd, const std::function<void(Time)>& drawFunction = {});
}
