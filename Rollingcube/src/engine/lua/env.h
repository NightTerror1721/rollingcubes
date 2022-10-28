#pragma once

#include <memory>

#include "natives.h"


class LuaEnv
{
private:
	std::unique_ptr<LuaRef> _env = nullptr;

public:
	LuaEnv() = default;
	LuaEnv(const LuaEnv&) = default;
	LuaEnv(LuaEnv&&) noexcept = default;
	~LuaEnv() = default;

	LuaEnv& operator= (const LuaEnv&) = default;
	LuaEnv& operator= (LuaEnv&&) noexcept = default;

public:

};
