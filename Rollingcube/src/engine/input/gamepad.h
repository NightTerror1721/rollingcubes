#pragma once

#include <functional>
#include <algorithm>
#include <vector>

#include "common.h"


enum class GamepadButton
{
	A = GLFW_GAMEPAD_BUTTON_A,
	B = GLFW_GAMEPAD_BUTTON_B,
	X = GLFW_GAMEPAD_BUTTON_X,
	Y = GLFW_GAMEPAD_BUTTON_Y,
	LeftBumper = GLFW_GAMEPAD_BUTTON_LEFT_BUMPER,
	RightBumper = GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER,
	Back = GLFW_GAMEPAD_BUTTON_BACK,
	Start = GLFW_GAMEPAD_BUTTON_START,
	Guide = GLFW_GAMEPAD_BUTTON_GUIDE,
	LeftThumb = GLFW_GAMEPAD_BUTTON_LEFT_THUMB,
	RightThumb = GLFW_GAMEPAD_BUTTON_RIGHT_THUMB,
	DpadUp = GLFW_GAMEPAD_BUTTON_DPAD_UP,
	DpadRight = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,
	DpadDown = GLFW_GAMEPAD_BUTTON_DPAD_DOWN,
	DpadLeft = GLFW_GAMEPAD_BUTTON_DPAD_LEFT,

	Last = GLFW_GAMEPAD_BUTTON_LAST,

	LB = LeftBumper,
	RB = RightBumper,
	LT = LeftThumb,
	RT = RightThumb,

	Cross = A,
	Circle = B,
	Square = X,
	Triangle = Y
};

enum class GamepadAxis
{
	LeftX = GLFW_GAMEPAD_AXIS_LEFT_X,
	LeftY = GLFW_GAMEPAD_AXIS_LEFT_Y,
	RightX = GLFW_GAMEPAD_AXIS_RIGHT_X,
	RightY = GLFW_GAMEPAD_AXIS_RIGHT_Y,
	LeftTrigger = GLFW_GAMEPAD_AXIS_LEFT_TRIGGER,
	RightTrigger = GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER,

	Last = GLFW_GAMEPAD_AXIS_LAST
};

using GamepadAxisValue = float;
using GamepadButtonValue = ButtonState;

enum class GamepadJoystickId : int
{
	J1 = GLFW_JOYSTICK_1,
	J2 = GLFW_JOYSTICK_2,
	J3 = GLFW_JOYSTICK_3,
	J4 = GLFW_JOYSTICK_4,
	J5 = GLFW_JOYSTICK_5,
	J6 = GLFW_JOYSTICK_6,
	J7 = GLFW_JOYSTICK_7,
	J8 = GLFW_JOYSTICK_8,
	J9 = GLFW_JOYSTICK_9,
	J10 = GLFW_JOYSTICK_10,
	J11 = GLFW_JOYSTICK_11,
	J12 = GLFW_JOYSTICK_12,
	J13 = GLFW_JOYSTICK_13,
	J14 = GLFW_JOYSTICK_14,
	J15 = GLFW_JOYSTICK_15,
	J16 = GLFW_JOYSTICK_16,

	Last = J16
};

class GamepadManager;

class Gamepad
{
public:
	friend GamepadManager;

public:
	using Button = GamepadButton;
	using Axis = GamepadAxis;
	using ButtonValue = GamepadButtonValue;
	using AxisValue = GamepadAxisValue;

	using JoystickId = GamepadJoystickId;

	using ButtonUpdateFunction = std::function<void(JoystickId, Button, ButtonValue)>;
	using AxisUpdateFunction = std::function<void(JoystickId, Axis, AxisValue)>;

private:
	using State = GLFWgamepadstate;

private:
	JoystickId _joystickId = JoystickId(-1);
	bool _enabled = false;
	State _oldState = {};
	State _state = {};

public:
	Gamepad() = delete;
	Gamepad(const Gamepad&) = delete;
	Gamepad(Gamepad&&) noexcept = delete;
	~Gamepad() = default;

	Gamepad& operator= (const Gamepad&) = delete;
	Gamepad& operator= (Gamepad&&) noexcept = delete;

private:
	inline Gamepad(JoystickId playerId) : _joystickId(playerId) {}

private:
	inline void clear()
	{
		std::memset(&_oldState, 0, sizeof(_oldState));
		std::memset(&_state, 0, sizeof(_state));
	}
	inline void connect()
	{
		_enabled = true;
		clear();
	}
	inline void disconnect()
	{
		_enabled = false;
		clear();
	}

public:
	constexpr bool isEnabled() const { return _enabled; }
	constexpr JoystickId getJoystickId() const { return _joystickId; }

	inline void updateState()
	{
		if (isEnabled())
		{
			_oldState = _state;
			glfwGetGamepadState(static_cast<int>(_joystickId), &_state);
		}
	}

	inline void checkUpdates(const ButtonUpdateFunction& buttonsCallback, const AxisUpdateFunction& axesCallback)
	{
		updateState();

		for (int i = 0; i <= int(Button::Last); ++i)
			if (_state.buttons[i] != _oldState.buttons[i])
				buttonsCallback(_joystickId, Button(i), ButtonValue(_state.buttons[i]));

		for (int i = 0; i <= int(Axis::Last); ++i)
			if (_state.axes[i] != _oldState.axes[i])
				axesCallback(_joystickId, Axis(i), AxisValue(_state.axes[i]));
	}

public:
	constexpr ButtonValue getButtonState(Button button) const
	{
		return ButtonValue(int(_state.buttons[int(button)]));
	}
	constexpr bool isButtonPressed(Button button) const { return getButtonState(button) == ButtonValue::Pressed; }

	constexpr AxisValue getAxisValue(Axis axis) const
	{
		return AxisValue(_state.axes[int(axis)]);
	}
};


class GamepadManager
{
private:
	using JoystickId = Gamepad::JoystickId;
	using ButtonUpdateFunction = Gamepad::ButtonUpdateFunction;
	using AxisUpdateFunction = Gamepad::AxisUpdateFunction;

private:
	static inline Gamepad Gamepads[int(JoystickId::Last) + 1] = {
		Gamepad(JoystickId::J1), Gamepad(JoystickId::J2), Gamepad(JoystickId::J3), Gamepad(JoystickId::J4),
		Gamepad(JoystickId::J5), Gamepad(JoystickId::J6), Gamepad(JoystickId::J7), Gamepad(JoystickId::J8),
		Gamepad(JoystickId::J9), Gamepad(JoystickId::J10), Gamepad(JoystickId::J11), Gamepad(JoystickId::J12),
		Gamepad(JoystickId::J13), Gamepad(JoystickId::J14), Gamepad(JoystickId::J15), Gamepad(JoystickId::J16)
	};

	static inline std::vector<Gamepad*> EnabledGamepads = {};

public:
	GamepadManager() = delete;
	GamepadManager(const GamepadManager&) = delete;
	GamepadManager(GamepadManager&&) noexcept = delete;
	~GamepadManager() = delete;

	GamepadManager& operator= (const GamepadManager&) = delete;
	GamepadManager& operator= (GamepadManager&&) noexcept = delete;

private:
	static inline Gamepad* getPointer(JoystickId id) { return Gamepads + int(id); }
	static inline bool isEnabled(JoystickId jid)
	{
		const auto sentinel = EnabledGamepads.end();
		const auto it = std::find(EnabledGamepads.begin(), sentinel, getPointer(jid));
		return it != sentinel;
	}
	static inline Gamepad* erase(JoystickId jid)
	{
		const auto sentinel = EnabledGamepads.end();
		const auto it = std::find(EnabledGamepads.begin(), sentinel, getPointer(jid));
		if (it != sentinel)
		{
			Gamepad* pad = *it;
			EnabledGamepads.erase(it);
			return pad;
		}
		return nullptr;
	}
	static inline bool isValidGamepad(JoystickId jid) { return glfwJoystickIsGamepad(int(jid)); }

	static inline void joystickConnectionCallback(int jid, int event)
	{
		if (event == GLFW_CONNECTED)
		{
			if (!isEnabled(JoystickId(jid)) && isValidGamepad(JoystickId(jid)))
			{
				Gamepad* pad = getPointer(JoystickId(jid));
				EnabledGamepads.push_back(pad);
				std::sort(EnabledGamepads.begin(), EnabledGamepads.end());

				pad->connect();
			}
		}
		else if (event == GLFW_DISCONNECTED)
		{
			Gamepad* pad = erase(JoystickId(jid));
			if (pad != nullptr)
				pad->disconnect();
		}
	}

public:
	static inline void checkUpdates(const ButtonUpdateFunction& buttonsCallback, const AxisUpdateFunction& axesCallback)
	{
		static constinit bool initiated = false;
		if (!initiated)
		{
			glfwSetJoystickCallback(&joystickConnectionCallback);
			initiated = true;
		}

		for (Gamepad* pad : EnabledGamepads)
			pad->checkUpdates(buttonsCallback, axesCallback);
	}
};
