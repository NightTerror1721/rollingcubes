#pragma once

#include "core/gl.h"
#include "core/window.h"

enum class ButtonState
{
	Released = GLFW_RELEASE,
	Pressed = GLFW_PRESS,
	Repeated = GLFW_REPEAT
};

namespace input
{
	constexpr ButtonState intToState(int state) { return static_cast<ButtonState>(state); }
	constexpr int stateToInt(ButtonState state) { return static_cast<int>(state); }
}
