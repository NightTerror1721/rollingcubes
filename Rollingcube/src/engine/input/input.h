#pragma once

#include <functional>

#include "event.h"


class InputManager
{
public:
	using DispatcherFunction = std::function<void(const InputEvent&)>;

private:
	struct EventHandler
	{
		InputEvent event;
		DispatcherFunction dispatcher;
	};

private:
	static inline EventHandler Handler = {};

private:
	InputManager() = delete;
	InputManager(const InputManager&) = delete;
	InputManager(InputManager&&) noexcept = delete;
	~InputManager() = delete;

	InputManager& operator= (const InputManager&) = delete;
	InputManager& operator= (InputManager&&) noexcept = delete;

private:
	static void keyEventCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void textEventCallback(GLFWwindow* window, unsigned int codepoint);
	static void mouseMoveEventCallback(GLFWwindow* window, double xpos, double ypos);
	static void mouseButtonEventCallback(GLFWwindow* window, int button, int action, int mods);
	static void mouseScrollEventCallback(GLFWwindow* window, double xoffset, double yoffset);
	static void gamepadButtonEventCallback(Gamepad::JoystickId jid, Gamepad::Button button, Gamepad::ButtonValue value);
	static void gamepadAxisEventCallback(Gamepad::JoystickId jid, Gamepad::Axis axis, Gamepad::AxisValue value);

public:
	static void dispatchEvents(const DispatcherFunction& dispatcherFunction);
};
