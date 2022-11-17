#include "chunk.h"

#include "constants.h"
#include "manager.h"
#include "calls.h"


LuaChunk::LuaChunk(lua_State* state, const Path& path, LuaChunk* parent) :
	_state(state),
	_path(resources::absolute(path)),
	_chunk(state),
	_env(parent == nullptr ? std::make_shared<LuaEnv>(state) : parent->_env),
	_id(),
	_dirPath(),
	_parent(parent),
	_includes()
{
	_dirPath = Path(_path).remove_filename();
	_id = _path.string();
	load();
}

void LuaChunk::run() const
{
	if (isValid())
	{
		prepareEnv(_state, *_env);

		_chunk.push();
		_env->push();
		lua_setupvalue(_state, -2, 1);

		LuaCallManager::instance().push(const_cast<LuaChunk*>(this));
		lua::utils::pcall(_state);
		LuaCallManager::instance().pop();
	}
}

bool LuaChunk::load()
{
	bool status = lua::utils::catchError(_state, luaL_loadfile(_state, _path.string().c_str()));
	if (status)
	{
		_chunk = LuaRef::fromStack(_state);
		return true;
	}

	destroy();
	return false;
}

bool LuaChunk::reload()
{
	if (isValid())
	{
		if (_parent != nullptr)
			throw std::exception("Child Chunks cannot reloaded");

		_env->clear();
		return true;
	}
	return false;
}

void LuaChunk::destroy()
{
	_state = nullptr;
	_path = Path();
	_dirPath = Path();
	_id = {};
	_chunk = luabridge::Nil();
	_parent = nullptr;
	_includes.clear();
}

void LuaChunk::prepareEnv(lua_State* state, const LuaEnv& env)
{
	env[lua::lib::constants::import] = luabridge::getGlobal(state, lua::lib::constants::import);
	env[lua::lib::constants::include] = luabridge::getGlobal(state, lua::lib::constants::include);
	env[lua::lib::constants::openlib] = luabridge::getGlobal(state, lua::lib::constants::openlib);
}

bool LuaChunk::checkRecursiveInclusion(const Path& abspath)
{
	LuaChunk* parent = _parent;
	while (parent != nullptr)
	{
		if (parent->_includes.contains(abspath))
		{
			lua::utils::error(_state, "Parent script {} already include {}. Recursive inclusion is forbiden.",
				parent->_path.string(), abspath.string());
			return false;
		}
		parent = parent->_parent;
	}
	return true;
}

bool LuaChunk::includeSubChunk(const Path& path)
{
	if (isValid())
	{
		Path abspath = resources::absolute(path);
		LuaChunk* chunk;
		auto it = _includes.find(abspath);
		if (it != _includes.end())
			chunk = it->second.get();
		else
		{
			std::unique_ptr<LuaChunk> ptr{ new LuaChunk(_state, abspath, this) };
			if (!ptr->isValid() || !checkRecursiveInclusion(abspath))
				return false;

			chunk = ptr.get();
			_includes.insert({ abspath, std::move(ptr) });
		}

		chunk->run();
		return true;
	}

	return false;
}
