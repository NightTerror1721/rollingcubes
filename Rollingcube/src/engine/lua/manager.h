#pragma once

#include <unordered_map>
#include <stack>

#include "script.h"
#include "global_state.h"


class LuaLibraryManager;

class LuaScriptManager
{
private:
	struct CallInfo
	{
		std::string scriptId;
		const LuaRef* currentEnv;
	};

private:
	lua_State* _state;
	std::unordered_map<std::string, std::shared_ptr<LuaChunk>> _chunks;
	std::stack<CallInfo> _callStack;

public:
	LuaScriptManager(const LuaScriptManager&) = delete;
	LuaScriptManager(LuaScriptManager&&) = delete;

	LuaScriptManager& operator= (const LuaScriptManager&) = delete;
	LuaScriptManager& operator= (LuaScriptManager&&) noexcept = delete;

private:
	LuaScriptManager();

	std::shared_ptr<LuaChunk> findChunk(const Path& path);

public:
	LuaScript getScript(std::string_view path);

	LuaScript getCurrentRunScript() const;

	const LuaRef* getCurrentRunScriptEnv() const;

public:
	inline lua_State* getLuaState() const { return _state; }

	inline void clear()
	{
		while (!_callStack.empty())
			_callStack.pop();
		_chunks.clear();
	}

private:
	inline ~LuaScriptManager() { clear(); }

private:
	static LuaScriptManager Instance;

public:
	static inline LuaScriptManager& instance() { return Instance; }

public:
	friend LuaScriptManagerLink;
	friend LuaLibraryManager;
};




inline void LuaScriptManagerLink::pushCall(const std::string& chunk, const LuaRef& currentEnv) const
{
	if (_manager != nullptr)
		_manager->_callStack.push({ chunk, std::addressof(currentEnv) });
}

inline void LuaScriptManagerLink::popCall() const
{
	if (_manager != nullptr)
		_manager->_callStack.pop();
}

inline LuaScriptManagerLink::LuaScriptManagerLink(LuaScriptManager& manager) :
	_manager(std::addressof(manager)),
	_state(manager._state)
{}
