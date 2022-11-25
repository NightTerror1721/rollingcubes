#pragma once

#include "natives.h"


class LuaGlobalState
{
private:
	lua_State* _state;

public:
	LuaGlobalState(const LuaGlobalState&) = delete;
	LuaGlobalState(LuaGlobalState&&) = delete;

	LuaGlobalState& operator= (const LuaGlobalState&) = delete;
	LuaGlobalState& operator= (LuaGlobalState&&) noexcept = delete;

private:
	LuaGlobalState();

public:
	inline ~LuaGlobalState()
	{
		if (_state != nullptr)
			lua::utils::closeState(_state);
		_state = nullptr;
	}

	inline lua_State* get() const { return _state; }

private:
	static LuaGlobalState Instance;

public:
	static inline lua_State* state() { return Instance.get(); }
};
