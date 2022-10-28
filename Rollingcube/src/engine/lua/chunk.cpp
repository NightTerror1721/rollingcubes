#include "chunk.h"

#include "constants.h"
#include "manager.h"


LuaChunk::LuaChunk(const LuaScriptManagerLink& managerLink, const Path& path) :
	_manager(managerLink),
	_path(resources::absolute(path)),
	_chunk(std::make_shared<LuaRef>(managerLink.getLuaState())),
	_env(std::make_shared<LuaRef>(managerLink.getLuaState())),
	_id(),
	_dirPath()
{
	_dirPath = Path(_path).remove_filename();
	_id = _path.string();
	load();
}

const std::shared_ptr<LuaRef>& LuaChunk::getEnv()
{
	static const std::shared_ptr<LuaRef> invalid_ptr = nullptr;

	if (isValid())
		return _env;
	return invalid_ptr;
}

std::shared_ptr<const LuaRef> LuaChunk::getEnv() const
{
	static const std::shared_ptr<LuaRef> invalid_ptr = nullptr;

	if (isValid())
		return _env;
	return invalid_ptr;
}

void LuaChunk::run(const LuaRef* customEnv) const
{
	if (isValid())
	{
		const LuaRef& env = customEnv ? *customEnv : *_env;
		lua_State* state = _manager.getLuaState();

		prepareEnv(state, env);

		_chunk->push();
		env.push();
		lua_setupvalue(state, -2, 1);

		_manager.pushCall(_id, env);
		lua::utils::pcall(state);
		_manager.popCall();
	}
}

bool LuaChunk::load()
{
	lua_State* state = _manager.getLuaState();
	bool status = lua::utils::catchError(state, luaL_loadfile(state, _path.string().c_str()));
	if (status)
	{
		*_chunk = LuaRef::fromStack(state);
		*_env = LuaRef::newTable(state);
//		lua_pop(state, 1);
		return true;
	}

	destroy();
	return false;
}

bool LuaChunk::reload()
{
	if (isValid())
	{
		lua_State* state = _manager.getLuaState();
		*_env = LuaRef::newTable(state);
		return true;
	}
	return false;
}

void LuaChunk::destroy()
{
	_manager = {};
	_path = Path();
	_dirPath = Path();
	_id = {};
	_chunk.reset();
	_env.reset();
}

void LuaChunk::prepareEnv(lua_State* state, const LuaRef& env)
{
	env[lua::lib::constants::import] = luabridge::getGlobal(state, lua::lib::constants::import);
	env[lua::lib::constants::openlib] = luabridge::getGlobal(state, lua::lib::constants::openlib);
}
