#pragma once

#include <unordered_map>

#include "utils/logger.h"

#include "keyboard.h"
#include "mouse.h"
#include "gamepad.h"


enum class InputUniqueIdType
{
	Invalid = 0,
	Key,
	MouseButton,
	GamepadButton,
	GamepadAxis
};


class InputUniqueId
{
public:
	using IntegerType = std::uint32_t;
	using Type = InputUniqueIdType;

private:
	static constexpr int TypeBits = 3;
	static constexpr int TypeAndJoystickIdBits = TypeBits + 4;

private:
	IntegerType _raw = 0;

public:
	constexpr InputUniqueId() = default;
	constexpr InputUniqueId(const InputUniqueId&) = default;
	constexpr InputUniqueId(InputUniqueId&&) noexcept = default;
	constexpr ~InputUniqueId() = default;

	constexpr InputUniqueId& operator= (const InputUniqueId&) = default;
	constexpr InputUniqueId& operator= (InputUniqueId&&) noexcept = default;

	constexpr bool operator== (const InputUniqueId& right) const = default;
	constexpr auto operator<=> (const InputUniqueId& right) const = default;

private:
	static constexpr IntegerType prepare(Type type) { return IntegerType(type) & 0b111u; }
	static constexpr IntegerType prepare(Key key) { return (IntegerType(key) & 0b111111111u) << TypeBits; }
	static constexpr IntegerType prepare(Mouse::Button button) { return (IntegerType(button) & 0b111u) << TypeBits; }
	static constexpr IntegerType prepare(Gamepad::JoystickId jId) { return (IntegerType(jId) & 0b1111u) << TypeBits; }
	static constexpr IntegerType prepare(Gamepad::Button button) { return (IntegerType(button) & 0b1111u) << TypeAndJoystickIdBits; }
	static constexpr IntegerType prepare(Gamepad::Axis axis) { return (IntegerType(axis) & 0b111u) << TypeAndJoystickIdBits; }

private:
	template <typename _Ty> requires std::same_as<_Ty, Key> || std::same_as<_Ty, Mouse::Button>
	static constexpr IntegerType prepare(Type type, _Ty value) { return prepare(type) | prepare(value); }

	template <typename _Ty> requires std::same_as<_Ty, Gamepad::Button> || std::same_as<_Ty, Gamepad::Axis>
	static constexpr IntegerType prepare(Type type, Gamepad::JoystickId jId, _Ty value) { return prepare(type) | prepare(jId) | prepare(value); }

public:
	constexpr InputUniqueId(Key key) : _raw(prepare(Type::Key, key)) {}
	constexpr InputUniqueId(Mouse::Button button) : _raw(prepare(Type::MouseButton, button)) {}
	constexpr InputUniqueId(Gamepad::JoystickId jId, Gamepad::Button button) : _raw(prepare(Type::GamepadButton, jId, button)) {}
	constexpr InputUniqueId(Gamepad::JoystickId jId, Gamepad::Axis axis) : _raw(prepare(Type::GamepadAxis, jId, axis)) {}

public:
	constexpr bool isValid() const { return _raw == 0; }

public:
	constexpr operator bool() const { return isValid(); }
	constexpr bool operator! () const { return !isValid(); }

public:
	friend std::hash<InputUniqueId>;
};


namespace std
{
	template <>
	struct hash<InputUniqueId> : hash<InputUniqueId::IntegerType>
	{
		inline std::size_t operator() (const InputUniqueId& id) const
		{
			return hash<InputUniqueId::IntegerType>::_Do_hash(id._raw);
		}
	};
}


class InputBind
{
private:
	using Id = unsigned int;

private:
	Id _id = 0;

public:
	constexpr InputBind() = default;
	constexpr InputBind(const InputBind&) = default;
	constexpr InputBind(InputBind&&) noexcept = default;
	constexpr ~InputBind() = default;

	constexpr InputBind& operator= (const InputBind&) = default;
	constexpr InputBind& operator= (InputBind&&) noexcept = default;

	constexpr bool operator== (const InputBind&) const = default;
	constexpr auto operator<=> (const InputBind&) const = default;

public:
	constexpr bool isValid() const { return _id != 0; }

public:
	constexpr operator bool() const { return isValid(); }
	constexpr bool operator! () const { return !isValid(); }

public:
	static inline InputBind make()
	{
		static constinit Id idgen = 0;
		InputBind bind;
		bind._id = ++idgen;
		return bind;
	}
};


class InputBindManager
{
private:
	static InputBindManager Instance;

private:
	std::unordered_map<InputUniqueId, InputBind> _binds;

private:
	InputBindManager() = default;
	InputBindManager(const InputBindManager&) = delete;
	InputBindManager(InputBindManager&&) noexcept = delete;
	~InputBindManager() = default;

	InputBindManager& operator= (const InputBindManager&) = delete;
	InputBindManager& operator= (InputBindManager&&) noexcept = delete;

public:
	static inline bool registerBind(const InputUniqueId& inputId, InputBind bind)
	{
		if (!inputId)
		{
			logger::error("Cannot register invalid InputUniqueId");
			return false;
		}

		if (!bind)
		{
			logger::error("Cannot register invalid InputBind");
			return false;
		}

		Instance._binds.insert({ inputId, bind });
		return true;
	}

	static inline InputBind getBind(const InputUniqueId& inputId)
	{
		const auto& it = Instance._binds.find(inputId);
		if (it == Instance._binds.end())
			return InputBind();
		return it->second;
	}

	static inline bool hasBind(const InputUniqueId& inputId)
	{
		return Instance._binds.contains(inputId);
	}
};

inline InputBindManager InputBindManager::Instance = {};


struct InputBindValue
{
	ButtonState state = ButtonState::Released;
	Gamepad::AxisValue axisValue = 0;

public:
	constexpr InputBindValue() = default;
	constexpr InputBindValue(const InputBindValue&) = default;
	constexpr InputBindValue(InputBindValue&&) noexcept = default;
	constexpr ~InputBindValue() = default;

	constexpr InputBindValue& operator= (const InputBindValue&) = default;
	constexpr InputBindValue& operator= (InputBindValue&&) noexcept = default;

public:
	constexpr InputBindValue(ButtonState state) : state(state), axisValue(state == ButtonState::Pressed ? 1.f : 0.f) {}
	constexpr InputBindValue(Gamepad::AxisValue value) : state(value != 0 ? ButtonState::Pressed : ButtonState::Released), axisValue(value) {}

public:
	constexpr bool isPressed() const { return state == ButtonState::Pressed; }
	constexpr bool isReleased() const { return state == ButtonState::Released; }

	constexpr bool operator== (decltype(nullptr)) const { return axisValue == 0; }
	constexpr bool operator!= (decltype(nullptr)) const { return axisValue != 0; }
};


struct InputBindState
{
	InputBind bind;
	InputBindValue value;
};
