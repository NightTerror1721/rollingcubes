#include "manager.h"

#include "global_state.h"
#include "libs.h"


LuaScriptManager LuaScriptManager::Instance = {};

LuaScriptManager::LuaScriptManager() :
	_state(LuaGlobalState::state()),
	_chunks(),
	_callStack()
{}

std::shared_ptr<LuaChunk> LuaScriptManager::findChunk(const Path& path)
{
	Path absPath = resources::absolute(path);
	auto it = _chunks.find(absPath.string());
	if (it != _chunks.end())
		return it->second;

	std::shared_ptr<LuaChunk> chunk = std::make_shared<LuaChunk>(LuaScriptManagerLink(*this), absPath);
	if (chunk->isValid())
	{
		_chunks.insert({ chunk->getId(), chunk });
		return chunk;
	}

	return nullptr;
}

LuaScript LuaScriptManager::getScript(std::string_view path)
{
	auto chunk = findChunk(path);
	if (chunk)
		return LuaScript(chunk);
	return LuaScript();
}

LuaScript LuaScriptManager::getCurrentRunScript() const
{
	if (!_callStack.empty())
	{
		const auto& info = _callStack.top();
		auto it = _chunks.find(info.scriptId);
		if (it != _chunks.end())
			return LuaScript(it->second);
	}
	return LuaScript();
}

const LuaRef* LuaScriptManager::getCurrentRunScriptEnv() const
{
	if (!_callStack.empty())
	{
		const auto& info = _callStack.top();
		return info.currentEnv;
	}
	return nullptr;
}



LuaGlobalState::LuaGlobalState() :
	_state(lua::utils::newState())
{
	LuaLibraryManager::instance().loadBuiltInData(_state);
}

LuaGlobalState LuaGlobalState::Instance = {};
