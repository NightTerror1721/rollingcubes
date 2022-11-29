#pragma once

#include <stack>

#include "chunk.h"


class LuaCallManager
{
private:
	static LuaCallManager Instance;

private:
	std::stack<Reference<LuaChunk>> _callStack;

public:
	LuaCallManager(const LuaCallManager&) = delete;
	LuaCallManager(LuaCallManager&&) noexcept = delete;

	LuaCallManager& operator= (const LuaCallManager&) = delete;
	LuaCallManager& operator= (LuaCallManager&&) noexcept = delete;

private:
	LuaCallManager() = default;
	~LuaCallManager() = default;

public:
	inline void push(Reference<LuaChunk> chunk) { _callStack.push(chunk); }
	inline void pop()
	{
		if (_callStack.empty())
			throw std::exception("Cannot pop element from empty lua call stack");

		_callStack.pop();
	}
	inline LuaScript getCurrentScript() const
	{
		if (_callStack.empty())
			return LuaScript();
		return _callStack.top();
	}

public:
	static inline LuaCallManager& instance() { return Instance; }
};

inline LuaCallManager LuaCallManager::Instance = {};
