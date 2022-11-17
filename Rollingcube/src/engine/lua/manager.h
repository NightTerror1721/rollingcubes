#pragma once

#include <unordered_map>
#include <stack>

#include "script.h"
#include "global_state.h"


class LuaLibraryManager;

class LuaScriptManager
{
private:
	lua_State* _state;
	std::unordered_map<std::string, std::unique_ptr<LuaChunk>> _chunks;

public:
	LuaScriptManager(const LuaScriptManager&) = delete;
	LuaScriptManager(LuaScriptManager&&) = delete;

	LuaScriptManager& operator= (const LuaScriptManager&) = delete;
	LuaScriptManager& operator= (LuaScriptManager&&) noexcept = delete;

private:
	LuaScriptManager();

	Reference<LuaChunk> findChunk(const Path& path);

public:
	LuaScript getScript(std::string_view path);

	LuaScript getCurrentRunScript() const;

	const LuaEnv* getCurrentRunScriptEnv() const;

public:
	inline lua_State* getLuaState() const { return _state; }

	inline void clear()
	{
		_chunks.clear();
	}

private:
	inline ~LuaScriptManager() { clear(); }

private:
	static LuaScriptManager Instance;

public:
	static inline LuaScriptManager& instance() { return Instance; }

public:
	friend LuaLibraryManager;
};
