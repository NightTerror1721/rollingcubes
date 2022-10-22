#pragma once

#include <string>
#include <memory>

#include "utils/resources.h"

#include "natives.h"
#include "link.h"


class LuaChunk
{
private:
	static constexpr std::string_view ScriptIdUp = "__scriptid__";

	std::string _id;
	Path _path;
	Path _dirPath;
	std::shared_ptr<LuaRef> _chunk;
	std::shared_ptr<LuaRef> _env;
	LuaScriptManagerLink _manager;

public:
	LuaChunk() = default;
	LuaChunk(const LuaChunk&) = delete;
	LuaChunk(LuaChunk&&) noexcept = delete;
	inline ~LuaChunk() { destroy(); };

	LuaChunk& operator= (const LuaChunk&) = delete;
	LuaChunk& operator= (LuaChunk&&) noexcept = delete;

public:
	LuaChunk(const LuaScriptManagerLink& managerLink, const Path& path);

	const std::shared_ptr<LuaRef>& getEnv();
	std::shared_ptr<const LuaRef> getEnv() const;

	void run(const LuaRef* customEnv = nullptr) const;

public:
	inline const std::string& getId() const { return _id; }
	inline const Path& getPath() const { return _path; }
	inline const Path& getDirectory() const { return _dirPath; }
	inline lua_State* getLuaState() const { return _manager.getLuaState(); }

	inline bool isValid() const { return _manager.isValid() && !_id.empty(); }

private:
	bool load();
	void destroy();

	static void prepareEnv(lua_State* state, const LuaRef& env);
};
