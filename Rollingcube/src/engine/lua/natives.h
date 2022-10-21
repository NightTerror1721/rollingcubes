#pragma once

#include <string>

#include <nativelua/lua.hpp>
#include <LuaBridge/LuaBridge.h>

#include "utils/logger.h"
#include "utils/string_utils.h"


using luabridge::LuaRef;
using luabridge::LuaException;


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

	inline std::string extractErrorFromStack(lua_State* state)
	{
		if (lua_gettop(state) > 0)
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
			logger::error("LUA error: {}", extractErrorFromStack(state));
			return false;
		}
		return true;
	}
}
