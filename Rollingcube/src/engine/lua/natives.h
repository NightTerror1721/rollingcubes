#pragma once

#include <string>
#include <iostream>

#pragma warning(push)
#pragma warning(disable: 26495)
#define LUABRIDGE_CXX17
#include <nativelua/lua.hpp>
#include <LuaBridge/LuaBridge.h>
#pragma warning(pop)

#include "utils/logger.h"
#include "utils/string_utils.h"


using luabridge::LuaRef;
using luabridge::LuaException;

class LuaEnv;


namespace lua::utils
{
	inline lua_State* newState() { return luaL_newstate(); }

	inline void closeState(lua_State* state) { lua_close(state); }

	inline void openDefaultLibs(lua_State* state) { luaL_openlibs(state); }

	template <typename... _ArgsTys>
	inline void error(lua_State* state, std::string_view fmt, _ArgsTys&&... args)
	{
		luaL_error(state, ::utils::str::format(fmt, std::forward<_ArgsTys>(args)...).c_str());
	}
}

namespace lua::utils
{
	template <typename... _ArgsTys>
	inline void safeVCall(const LuaRef& ref, _ArgsTys&&... args)
	{
		try
		{
			ref(std::forward<_ArgsTys>(args)...);
		}
		catch (const LuaException& ex)
		{
			logger::error("LUA call error: {}", ex.what());
		}
	}

	template <typename... _ArgsTys>
	inline LuaRef safeCall(const LuaRef& ref, _ArgsTys&&... args)
	{
		try
		{
			return ref(std::forward<_ArgsTys>(args)...);
		}
		catch (const LuaException& ex)
		{
			logger::error("LUA call error: {}", ex.what());
		}
	}
}

namespace lua::utils
{
	inline std::string toString(lua_State* state, int index)
	{
		std::size_t len;
		const char* cstr = lua_tolstring(state, index, &len);
		return std::string(cstr, len);
	}

	inline std::string extractErrorFromStack(lua_State* state, int status)
	{
		if (lua_gettop(state) >= 0)
		{
			char const* s = lua_tostring(state, -1);
			return std::string(s ? s : "");
		}
		return std::string("missing error");
	}

	inline bool catchError(lua_State* state, int status)
	{
		if (status != LUA_OK)
		{
			logger::error("LUA error: {}", extractErrorFromStack(state, status));
			return false;
		}
		return true;
	}

	inline bool pcall(lua_State* state, int nargs = 0, int nresults = 0, int msgh = 0)
	{
		try
		{
			LuaException::pcall(state, nargs, nresults, msgh);
			return true;
		}
		catch (const LuaException& ex)
		{
			logger::error("LUA error: {}", ex.what());
			return false;
		}
	}
}

namespace lua::utils
{
	void dump(std::ostream& os, lua_State* state, const LuaRef& ref, unsigned int identation = 0, unsigned int identationIncrement = 4);
	void dump(std::ostream& os, lua_State* state, const LuaEnv& env, unsigned int identation = 0, unsigned int identationIncrement = 4);
}
