#pragma once

#include "keyboard.h"
#include "mouse.h"
#include "gamepad.h"
#include "binds.h"


enum class InputEventType
{
	KeyPressed,
	KeyReleased,
	TextEntered,

	MouseMoved,
	MouseButtonPressed,
	MouseButtonReleased,
	MouseWheelScrolled,

	GamepadButtonPressed,
	GamepadButtonReleased,
	GamepadAxisMoved
};

class InputEvent
{
public:
	struct KeyEvent
	{
		Key key;
		bool alt;
		bool control;
		bool shift;
		bool menu;
	};

	struct TextEvent
	{
		CodePoint codepoint;
	};

	struct MouseMoveEvent
	{
		Mouse::Position position;
	};

	struct MouseButtonEvent
	{
		Mouse::Button button;
	};

	struct MouseWheelScrollEvent
	{
		Mouse::Position offsets;
	};

	struct GamepadButtonEvent
	{
		Gamepad::JoystickId joystickId;
		Gamepad::Button button;
	};

	struct GamepadAxisEvent
	{
		Gamepad::JoystickId joystickId;
		Gamepad::Axis axis;
	};

public:
	using Type = InputEventType;

public:
	Type type;

	union
	{
		KeyEvent key;
		TextEvent text;
		MouseMoveEvent mouseMove;
		MouseButtonEvent mouseButton;
		MouseWheelScrollEvent mouseWheel;
		GamepadButtonEvent gamepadButton;
		GamepadAxisEvent gamepadAxis;
	};

	InputBindState bindState;
};
