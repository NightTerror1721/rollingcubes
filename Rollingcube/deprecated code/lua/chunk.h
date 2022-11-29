#pragma once

#include <string>
#include <memory>
#include <unordered_map>

#include "utils/resources.h"

#include "natives.h"
#include "env.h"


class LuaChunk
{
private:
	std::string _id;
	Path _path;
	Path _dirPath;
	LuaRef _chunk;
	std::shared_ptr<LuaEnv> _env;
	lua_State* _state;

	LuaChunk* _parent;
	std::unordered_map<Path, std::unique_ptr<LuaChunk>> _includes;

public:
	LuaChunk() = default;
	LuaChunk(const LuaChunk&) = delete;
	LuaChunk(LuaChunk&&) noexcept = delete;
	inline ~LuaChunk() { destroy(); };

	LuaChunk& operator= (const LuaChunk&) = delete;
	LuaChunk& operator= (LuaChunk&&) noexcept = delete;

private:
	LuaChunk(lua_State* state, const Path& path, LuaChunk* parent);

public:
	inline LuaChunk(lua_State* state, const Path& path) :
		LuaChunk(state, path, nullptr)
	{}

public:
	void run() const;

	bool reload();

	bool includeSubChunk(const Path& path);

public:
	inline const std::string& getId() const { return _id; }
	inline const Path& getPath() const { return _path; }
	inline const Path& getDirectory() const { return _dirPath; }
	inline lua_State* getLuaState() const { return _state; }
	inline const LuaEnv& getEnv() const { return *_env; }

	inline bool isValid() const { return !_id.empty(); }

private:
	bool load();
	void destroy();

	bool checkRecursiveInclusion(const Path& abspath);

	static void prepareEnv(lua_State* state, const LuaEnv& env);
};
