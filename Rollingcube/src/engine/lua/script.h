#pragma once

#include <initializer_list>
#include <concepts>
#include <ranges>

#include "utils/reference.h"

#include "chunk.h"


class LuaScript
{
private:
	Reference<LuaChunk> _chunk = nullptr;

public:
	constexpr LuaScript() = default;
	constexpr LuaScript(const LuaScript&) = default;
	constexpr LuaScript(LuaScript&&) noexcept = default;
	constexpr ~LuaScript() = default;

	constexpr LuaScript& operator= (const LuaScript&) = default;
	constexpr LuaScript& operator= (LuaScript&&) noexcept = default;

public:
	inline LuaScript(Reference<LuaChunk> chunk) : _chunk(chunk) {}

	inline bool isValid() const { return _chunk != nullptr && _chunk->isValid(); }

	inline operator bool() const { return isValid(); }
	inline bool operator! () const { return !isValid(); }

	inline Path getPath() const { return isValid() ? _chunk->getPath() : Path(); }
	inline Path getDirectory() const { return isValid() ? _chunk->getDirectory() : Path(); }

	inline const LuaEnv& getEnv() const { return _chunk->getEnv(); }

	inline bool includeSubScript(const Path& path) { return _chunk->includeSubChunk(path); }

	inline bool reload() { return isValid() ? _chunk->reload() : false; }

public:
	inline void operator() () const
	{
		if (_chunk != nullptr)
			_chunk->run();
	}
};
