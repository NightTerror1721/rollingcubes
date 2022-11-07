#pragma once

#include "common.h"

#include "math/glm.h"


enum class MouseButton
{
	Button1 = GLFW_MOUSE_BUTTON_1,
	Button2 = GLFW_MOUSE_BUTTON_2,
	Button3 = GLFW_MOUSE_BUTTON_3,
	Button4 = GLFW_MOUSE_BUTTON_4,
	Button5 = GLFW_MOUSE_BUTTON_5,
	Button6 = GLFW_MOUSE_BUTTON_6,
	Button7 = GLFW_MOUSE_BUTTON_7,
	Button8 = GLFW_MOUSE_BUTTON_8,

	Last = Button8,
	Left = Button1,
	Right = Button2,
	Middle = Button3
};

enum class MouseMode
{
	Normal = GLFW_CURSOR_NORMAL,
	Hidden = GLFW_CURSOR_HIDDEN,
	Dissabled = GLFW_CURSOR_DISABLED
};

struct MousePosition
{
	double x = 0;
	double y = 0;

public:
	constexpr MousePosition() = default;
	constexpr MousePosition(const MousePosition&) = default;
	constexpr MousePosition(MousePosition&&) noexcept = default;
	constexpr ~MousePosition() = default;

	constexpr MousePosition& operator= (const MousePosition&) = default;
	constexpr MousePosition& operator= (MousePosition&&) noexcept = default;

public:
	constexpr MousePosition(double x, double y) : x(x), y(y) {}
	constexpr MousePosition(const glm::ivec2& pos) : MousePosition(double(pos.x), double(pos.y)) {}
	constexpr MousePosition(const glm::uvec2& pos) : MousePosition(double(pos.x), double(pos.y)) {}
	constexpr MousePosition(const glm::vec2& pos) : MousePosition(double(pos.x), double(pos.y)) {}

	constexpr explicit operator glm::ivec2() const { return { int(x), int(y) }; }
	constexpr explicit operator glm::uvec2() const { return { unsigned int(x), unsigned int(y) }; }
	constexpr explicit operator glm::vec2() const { return { float(x), float(y) }; }
};

class Mouse
{
public:
	using Button = MouseButton;
	using Mode = MouseMode;
	using Position = MousePosition;

private:
	Mouse() = delete;
	Mouse(const Mouse&) = delete;
	Mouse(Mouse&&) noexcept = delete;
	~Mouse() = delete;

	Mouse& operator= (const Mouse&) = delete;
	Mouse& operator= (Mouse&&) noexcept = delete;

public:
	static inline bool isButtonPressed(Button button)
	{
		ButtonState state = input::intToState(glfwGetMouseButton(window::getMainWindow(), static_cast<int>(button)));
		return state == ButtonState::Pressed;
	}

	static inline Position getPosition()
	{
		Position pos;
		glfwGetCursorPos(window::getMainWindow(), &pos.x, &pos.y);
		return pos;
	}

	static inline void setPosition(const Position& pos)
	{
		glfwSetCursorPos(window::getMainWindow(), pos.x, pos.y);
	}

	static inline void setPositionToCenter()
	{
		window::Dimension dim = window::getMainWindowSize();
		setPosition({ double(dim.width), double(dim.height) });
	}

	static inline void setMode(Mode mode)
	{
		glfwSetInputMode(window::getMainWindow(), GLFW_CURSOR, static_cast<int>(mode));
	}

	static inline void hide() { setMode(Mode::Hidden); }
	static inline void disable() { setMode(Mode::Dissabled); }
	static inline void enable() { setMode(Mode::Normal); }
};
