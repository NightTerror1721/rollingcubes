#include "manager.h"

#include "libs.h"
#include "calls.h"


LuaGlobalState LuaGlobalState::Instance = {};

LuaGlobalState::LuaGlobalState() :
	_state(lua::utils::newState())
{
	lua::utils::openDefaultLibs(_state);
	LuaLibraryManager::loadBuiltInData(_state);
}




LuaScriptManager LuaScriptManager::Instance = {};

LuaScriptManager::LuaScriptManager() :
	_state(LuaGlobalState::state()),
	_chunks()
{}

Reference<LuaChunk> LuaScriptManager::findChunk(const Path& path)
{
	Path absPath = resources::absolute(path);
	auto it = _chunks.find(absPath.string());
	if (it != _chunks.end())
		return it->second.get();

	auto chunk = std::make_unique<LuaChunk>(_state, absPath);
	if (chunk->isValid())
	{
		Reference<LuaChunk> ref = chunk.get();
		_chunks.insert({ chunk->getId(), std::move(chunk) });
		return ref;
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
	return LuaCallManager::instance().getCurrentScript();
}

const LuaEnv* LuaScriptManager::getCurrentRunScriptEnv() const
{
	LuaScript script = LuaCallManager::instance().getCurrentScript();
	if (!script)
		return nullptr;

	return std::addressof(script.getEnv());
}
