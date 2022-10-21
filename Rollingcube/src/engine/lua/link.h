#pragma once

#include <string>

#include "natives.h"


class LuaScriptManager;
class LuaChunk;

class LuaScriptManagerLink
{
private:
	LuaScriptManager* _manager = nullptr;
	lua_State* _state = nullptr;

public:
	constexpr LuaScriptManagerLink() = default;
	constexpr LuaScriptManagerLink(const LuaScriptManagerLink&) = default;
	constexpr LuaScriptManagerLink(LuaScriptManagerLink&&) noexcept = default;
	constexpr ~LuaScriptManagerLink() = default;

	constexpr LuaScriptManagerLink& operator= (const LuaScriptManagerLink&) = default;
	constexpr LuaScriptManagerLink& operator= (LuaScriptManagerLink&&) noexcept = default;

public:
	void pushCall(const std::string& chunk, const LuaRef& currentEnv) const;
	void popCall() const;

public:
	constexpr lua_State* getLuaState() const { return _state; }
	constexpr bool isValid() const { return _state != nullptr && _manager != nullptr; }

public:
	constexpr operator bool() const { return isValid(); }
	constexpr bool operator! () const { return !isValid(); }

private:
	LuaScriptManagerLink(LuaScriptManager& manager);

public:
	friend LuaScriptManager;
};

