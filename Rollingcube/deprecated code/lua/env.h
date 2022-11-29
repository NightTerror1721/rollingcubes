#pragma once

#include <memory>

#include "natives.h"


template <typename _Ty>
concept LuaEnvValidKey = requires(const LuaRef & env, const _Ty& key)
{
	{ env[key] };
};

template <typename _Ty>
concept LuaEnvValidValue = requires(const LuaRef& env, const char* key, const _Ty& value)
{
	{ env[key] = value };
};

class LuaEnv
{
private:
	LuaRef _env;
	lua_State* _state;

public:
	LuaEnv(const LuaEnv&) = delete;
	LuaEnv(LuaEnv&&) noexcept = default;
	~LuaEnv() = default;

	LuaEnv& operator= (const LuaEnv&) = delete;
	LuaEnv& operator= (LuaEnv&&) noexcept = default;

public:
	inline explicit LuaEnv(lua_State* state) :
		_env(LuaRef::newTable(state)),
		_state(state)
	{}

public:
	inline const LuaRef& getLuaRef() const { return _env; }

	inline void clear() { _env = LuaRef::newTable(_state); }

	inline void push() const { _env.push(); }
	inline void pop() const { lua_pop(_state, 1); }

	template <LuaEnvValidKey _KeyTy, LuaEnvValidValue _ValTy>
	inline void set(const _KeyTy& name, const _ValTy& value) const { _env[name] = value; }

	template <LuaEnvValidValue _ValTy>
	inline void set(std::string_view name, const _ValTy& value) const { _env[name.data()] = value; }

	template <LuaEnvValidKey _KeyTy, LuaEnvValidValue _ValTy>
	inline _ValTy get(const _KeyTy& name) const
	{
		if constexpr (std::same_as<_ValTy, LuaRef>)
			return _env[name];
		else
			return _env[name].cast<_ValTy>();
	}

	template <LuaEnvValidValue _ValTy>
	inline _ValTy get(std::string_view name) const { return get<const char*, _ValTy>(name.data()); }

	inline void setFromGlobal(std::string_view globalName)
	{
		set(globalName, luabridge::getGlobal(_state, globalName.data()));
	}

private:
	template <typename _Ty> requires
		std::ranges::range<_Ty> &&
		(
			std::convertible_to<std::ranges::range_value_t<_Ty>, const char*> ||
			std::convertible_to<std::ranges::range_value_t<_Ty>, std::string> ||
			std::convertible_to<std::ranges::range_value_t<_Ty>, std::string_view>
			)
	void _setFromGlobal(const _Ty& names) const
	{
		const auto end = std::ranges::end(names);
		for (auto it = std::ranges::begin(names); it != end; ++it)
		{
			const char* name;
			if constexpr (std::convertible_to<std::ranges::range_value_t<_Ty>, std::string>)
				name = it->c_str();
			else if constexpr (std::convertible_to<std::ranges::range_value_t<_Ty>, std::string_view>)
				name = it->data();
			else
				name = static_cast<const char*>(*it);
			_env[name] = luabridge::getGlobal(_state, name);
		}
	}

public:
	template <typename _Ty> requires
		std::ranges::range<_Ty> &&
		(
			std::convertible_to<std::ranges::range_value_t<_Ty>, const char*> ||
			std::convertible_to<std::ranges::range_value_t<_Ty>, std::string> ||
			std::convertible_to<std::ranges::range_value_t<_Ty>, std::string_view>
			)
	inline void setFromGlobal(const _Ty& names) const
	{
		_setFromGlobal<_Ty>(names);
	}

	inline void setFromGlobal(std::initializer_list<std::string_view> names) const
	{
		_setFromGlobal(names);
	}

	template <std::convertible_to<std::string_view>... _ArgsTys>
	inline void setFromGlobal(_ArgsTys&&... names)
	{
		setFromGlobal({ std::forward<_ArgsTys>(names)... });
	}

public:
	template <LuaEnvValidKey _KeyTy>
	inline decltype(LuaRef(nullptr)[""]) operator[] (const _KeyTy& name) const { return _env[name]; }

	inline decltype(LuaRef(nullptr)[""]) operator[] (std::string_view name) const { return _env[name.data()]; }
};
