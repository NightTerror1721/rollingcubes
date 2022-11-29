#pragma once

#pragma warning(push)
#pragma warning(disable: 26495)
#define LUABRIDGE_CXX17
#include <nativelua/lua.hpp>
#include <LuaBridge/LuaBridge.h>
#pragma warning(pop)

#include <string>
#include <functional>

#include "utils/logger.h"
#include "utils/string_utils.h"


using luabridge::LuaRef;
using luabridge::LuaException;


namespace lua
{
	namespace bridge = ::luabridge;
}


template <typename _Ty>
concept LuaValidKey = requires(const LuaRef & env, const _Ty & key)
{
	{ env[key] };
};

template <typename _Ty>
concept LuaValidValue = requires(const LuaRef & env, const char* key, const _Ty & value)
{
	{ env[key] = value };
};


class LuaEnvironment
{
private:
	static const LuaEnvironment Instance;

private:
	lua_State* _state;

public:
	LuaEnvironment(const LuaEnvironment&) = delete;
	LuaEnvironment(LuaEnvironment&&) noexcept = delete;

	LuaEnvironment& operator= (const LuaEnvironment&) = delete;
	LuaEnvironment& operator= (LuaEnvironment&&) noexcept = delete;

public:
	inline LuaEnvironment() : _state(luaL_newstate())
	{
		luaL_openlibs(_state);
	}

	inline ~LuaEnvironment()
	{
		lua_close(_state);
	}

public:
	static constexpr const LuaEnvironment& instance() { return Instance; }

	constexpr lua_State* getLuaState() const { return _state; }
};

inline const LuaEnvironment LuaEnvironment::Instance = {};


namespace lua
{
	constexpr lua_State* state() { return LuaEnvironment::instance().getLuaState(); }
}

namespace lua::utils
{
	template <LuaValidValue _Ty>
	inline void push(const _Ty& value) { bridge::push<_Ty>(state(), value); }

	inline void pushNil() { push(bridge::Nil()); }

	inline void pop(unsigned int amount = 1) { lua_pop(state(), static_cast<int>(amount)); }

	inline LuaRef refFromStack(int index) { return LuaRef::fromStack(state(), index); }

	inline LuaRef newRef() { return LuaRef(state()); }

	inline LuaRef newTableRef() { return LuaRef::newTable(state()); }

	template <LuaValidValue _Ty>
	inline LuaRef newRef(const _Ty& value) { return LuaRef(state(), value); }

	template <LuaValidValue _Ty>
	inline LuaRef newRef(_Ty&& value) { return LuaRef(state(), std::move(value)); }

	template <typename... _ArgsTys>
	inline void error(std::string_view fmt, _ArgsTys&&... args)
	{
		luaL_error(state(), ::utils::str::format(fmt, std::forward<_ArgsTys>(args)...).c_str());
	}

	inline void forEachInLastTable(const std::function<void(const LuaRef&, const LuaRef&)>& action)
	{
		lua_State* ls = state();
		pushNil();
		while (lua_next(ls, -2))
		{
			action(refFromStack(-2), refFromStack(-1));

			pop();
		}

		pop();
	}

	inline void forEachInTable(const LuaRef& table, const std::function<void(const LuaRef&, const LuaRef&)>& action)
	{
		table.push();
		forEachInLastTable(action);
	}

	inline std::string extractErrorFromStack()
	{
		lua_State* ls = state();
		if (lua_gettop(ls) >= 0)
		{
			char const* s = lua_tostring(ls, -1);
			return std::string(s ? s : "");
		}
		return std::string("missing error");
	}

	inline bool catchError(int status)
	{
		if (status != LUA_OK)
		{
			logger::error("LUA error: {}", extractErrorFromStack());
			return false;
		}
		return true;
	}

	inline bool pcall(int nargs = 0, int nresults = 0, int msgh = 0)
	{
		try
		{
			LuaException::pcall(state(), nargs, nresults, msgh);
			return true;
		}
		catch (const LuaException& ex)
		{
			logger::error("LUA module error: {}", ex.what());
			return false;
		}
	}

	inline LuaRef getGlobalValue(std::string_view name) { return lua::bridge::getGlobal(state(), name.data()); }

	inline void setGlobalValue(std::string_view name, const LuaRef& value) { lua::bridge::setGlobal(state(), value, name.data()); }

	inline void pushGlobal(std::string_view name) { getGlobalValue(name).push(); }

	inline lua::bridge::Namespace getGlobalNamespace() { return lua::bridge::getGlobalNamespace(state()); }

	template <LuaValidValue _Ty>
	inline void setGlobalValue(std::string_view name, const _Ty& value) { lua::bridge::setGlobal<_Ty>(state(), value, name.data()); }

	inline void setGlobalNil(std::string_view name) { lua::bridge::setGlobal(state(), lua::bridge::Nil(), name.data()); }

	inline void setFromGlobal(std::string_view name, const LuaRef& env) { env[name] = getGlobalValue(name); }
}
