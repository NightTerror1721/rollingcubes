#include "input.h"

#define setInputBind(_UniqueId, _Value) \
{ \
	InputBind __bind = InputBindManager::getBind(InputUniqueId(_UniqueId)); \
	if(__bind.isValid()) { \
		Handler.event.bindState.bind = __bind; \
		Handler.event.bindState.value = InputBindValue(_Value); \
	} \
}

#define callDispatcher() Handler.dispatcher(Handler.event)


void InputManager::keyEventCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Handler.event.type = action == GLFW_PRESS ? InputEvent::Type::KeyPressed : InputEvent::Type::KeyReleased;
	Handler.event.key.key = static_cast<Key>(key);
	Handler.event.key.control = (mods & GLFW_MOD_SHIFT) != 0;
	Handler.event.key.alt = (mods & GLFW_MOD_ALT) != 0;
	Handler.event.key.shift = (mods & GLFW_MOD_SHIFT) != 0;
	Handler.event.key.menu = (mods & GLFW_MOD_SUPER) != 0;

	const ButtonState state = action == GLFW_RELEASE ? ButtonState::Released : ButtonState::Pressed;
	setInputBind(static_cast<Key>(key), state);
	callDispatcher();
}

void InputManager::textEventCallback(GLFWwindow* window, unsigned int codepoint)
{
	Handler.event.type = InputEvent::Type::TextEntered;
	Handler.event.text.codepoint = CodePoint(codepoint);

	callDispatcher();
}

void InputManager::mouseMoveEventCallback(GLFWwindow* window, double xpos, double ypos)
{
	Handler.event.type = InputEvent::Type::MouseMoved;
	Handler.event.mouseMove.position.x = xpos;
	Handler.event.mouseMove.position.y = ypos;

	callDispatcher();
}

void InputManager::mouseButtonEventCallback(GLFWwindow* window, int button, int action, int mods)
{
	Handler.event.type = action == GLFW_PRESS ? InputEvent::Type::MouseButtonPressed : InputEvent::Type::MouseButtonReleased;
	Handler.event.mouseButton.button = Mouse::Button(button);

	setInputBind(Mouse::Button(button), ButtonState(action));
	callDispatcher();
}

void InputManager::mouseScrollEventCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	Handler.event.type = InputEvent::Type::MouseWheelScrolled;
	Handler.event.mouseWheel.offsets.x = xoffset;
	Handler.event.mouseWheel.offsets.y = yoffset;

	callDispatcher();
}

void InputManager::gamepadButtonEventCallback(Gamepad::JoystickId jid, Gamepad::Button button, Gamepad::ButtonValue value)
{
	Handler.event.type = value == Gamepad::ButtonValue::Pressed ? InputEvent::Type::GamepadButtonPressed : InputEvent::Type::GamepadButtonReleased;
	Handler.event.gamepadButton.joystickId = jid;
	Handler.event.gamepadButton.button = button;

	setInputBind(InputUniqueId(jid, button), value);
	callDispatcher();
}

void InputManager::gamepadAxisEventCallback(Gamepad::JoystickId jid, Gamepad::Axis axis, Gamepad::AxisValue value)
{
	Handler.event.type = InputEvent::Type::GamepadAxisMoved;
	Handler.event.gamepadAxis.joystickId = jid;
	Handler.event.gamepadAxis.axis = axis;

	setInputBind(InputUniqueId(jid, axis), value);
	callDispatcher();
}

void InputManager::dispatchEvents(const DispatcherFunction& dispatcherFunction)
{
	static constinit bool initiated = false;
	if (!initiated)
	{
		glfwSetKeyCallback(window::getMainWindow(), &keyEventCallback);
		glfwSetCharCallback(window::getMainWindow(), &textEventCallback);
		glfwSetCursorPosCallback(window::getMainWindow(), &mouseMoveEventCallback);
		glfwSetMouseButtonCallback(window::getMainWindow(), &mouseButtonEventCallback);
		glfwSetScrollCallback(window::getMainWindow(), &mouseScrollEventCallback);

		initiated = true;
	}

	Handler.dispatcher = dispatcherFunction;

	glfwPollEvents();
	GamepadManager::checkUpdates(&gamepadButtonEventCallback, &gamepadAxisEventCallback);

	Handler.dispatcher = nullptr;
}
