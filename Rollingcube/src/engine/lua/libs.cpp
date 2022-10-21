#include "libs.h"

#include "constants.h"
#include "manager.h"


const LuaLibrary LuaLibrary::NativeBase = { NativeId::Base, "base" };
const LuaLibrary LuaLibrary::NativeCoroutine = { NativeId::Coroutine, LUA_COLIBNAME };
const LuaLibrary LuaLibrary::NativePackage = { NativeId::Package, LUA_LOADLIBNAME };
const LuaLibrary LuaLibrary::NativeString = { NativeId::String, LUA_STRLIBNAME };
const LuaLibrary LuaLibrary::NativeUtf8 = { NativeId::Utf8, LUA_UTF8LIBNAME };
const LuaLibrary LuaLibrary::NativeTable = { NativeId::Table, LUA_TABLIBNAME };
const LuaLibrary LuaLibrary::NativeMath = { NativeId::Math, LUA_MATHLIBNAME };
const LuaLibrary LuaLibrary::NativeIo = { NativeId::Io, LUA_IOLIBNAME };
const LuaLibrary LuaLibrary::NativeOs = { NativeId::Os, LUA_OSLIBNAME };
const LuaLibrary LuaLibrary::NativeDebug = { NativeId::Debug, LUA_DBLIBNAME };





LuaLibraryManager LuaLibraryManager::Instance = {};

static inline std::pair<std::string, LuaLibrary> insertlib(LuaLibrary::NativeId id)
{
	const LuaLibrary& lib = LuaLibrary::getNative(LuaLibrary::NativeId::Base);
	return { lib.getName(), lib };
}

#define customLibname(_Name) (std::string(GamelibPrefix) + _Name.data())

LuaLibraryManager::LuaLibraryManager() :
	_libs{
		std::move(insertlib(LuaLibrary::NativeId::Base)),
		std::move(insertlib(LuaLibrary::NativeId::Coroutine)),
		std::move(insertlib(LuaLibrary::NativeId::Package)),
		std::move(insertlib(LuaLibrary::NativeId::String)),
		std::move(insertlib(LuaLibrary::NativeId::Utf8)),
		std::move(insertlib(LuaLibrary::NativeId::Table)),
		std::move(insertlib(LuaLibrary::NativeId::Math)),
		std::move(insertlib(LuaLibrary::NativeId::Io)),
		std::move(insertlib(LuaLibrary::NativeId::Os)),
		std::move(insertlib(LuaLibrary::NativeId::Debug)),
	}
{}

void LuaLibraryManager::registerLibrary(std::string_view libraryName, const LuaLibraryConstructor& libraryConstructor)
{
	std::string name = customLibname(libraryName);
	if (_libs.contains(name))
	{
		logger::error("Lua Rollingcube library {} already registered.", libraryName);
		return;
	}

	_libs.insert({ name, LuaLibrary(name, libraryConstructor) });
	logger::info("Lua Rollingcube library {} registered correctly!", name);
}

void LuaLibraryManager::loadBuiltInData(lua_State* state)
{
	luabridge::getGlobalNamespace(state)
		.addFunction(lua::lib::constants::import.data(), &LUA_import)
		.addFunction(lua::lib::constants::openlib.data(), &LUA_openlib);
}

void LuaLibraryManager::openLibrary(lua_State* state, const std::string& libname) const
{
	const auto env = LuaScriptManager::instance().getCurrentRunScriptEnv();
	if (env == nullptr)
	{
		lua::utils::error(state, "Lua script environment not accesible.");
		return;
	}

	auto it = _libs.find(libname);
	if (it == _libs.end())
	{
		lua::utils::error(state, "Lua library {} not found.", libname);
		return;
	}

	const LuaLibrary& lib = it->second;
	if (lib.isNative())
		openLuaLibrary(state, *env, lib);
	else
		openCustomLibrary(state, *env, lib);
}

void LuaLibraryManager::openLuaLibrary(lua_State* state, const std::shared_ptr<LuaRef>& env, const LuaLibrary& lib)
{
	if (env == nullptr)
	{
		lua::utils::error(state, "NULL Environment on Lua open library routine.");
		return;
	}

	if (!lib.isLoaded())
	{
		lua_CFunction luaConstructor = nullptr;
		switch (lib.getNativeId())
		{
			using enum LuaLibrary::NativeId;

			case Base: luaConstructor = &luaopen_base;
			case Coroutine: luaConstructor = &luaopen_coroutine;
			case Package: luaConstructor = &luaopen_package;
			case String: luaConstructor = &luaopen_string;
			case Utf8: luaConstructor = &luaopen_utf8;
			case Table: luaConstructor = &luaopen_table;
			case Math: luaConstructor = &luaopen_math;
			case Io: luaConstructor = &luaopen_io;
			case Os: luaConstructor = &luaopen_os;
			case Debug: luaConstructor = &luaopen_debug;
		}

		if (luaConstructor == nullptr)
		{
			lua::utils::error(state, "Cannot open Lua Library {}: Unknown Lua Native Library ID {}.", lib.getName(), static_cast<int>(lib.getNativeId()));
			return;
		}

		LuaRef libTable = luabridge::getGlobal(state, lib.getName().c_str());
		if (!libTable.isTable())
		{
			luabridge::setGlobal(state, LuaRef::newTable(state), lib.getName().c_str());
			libTable = luabridge::getGlobal(state, lib.getName().c_str());
		}

		std::string_view libname = lib.getNativeId() == LuaLibrary::NativeId::Base ? "" : lib.getName();

		lua_pushcfunction(state, luaConstructor);
		lua_pushstring(state, libname.data());
		lua_call(state, 1, 1);

		luabridge::push(state, luabridge::Nil());
		while (lua_next(state, -2))
		{
			LuaRef key = LuaRef::fromStack(state, -2);
			LuaRef val = LuaRef::fromStack(state, -1);

			libTable[key] = val;
			lua_pop(state, 1);
		}

		lua_pop(state, 1);
	}

	LuaRef libTable = luabridge::getGlobal(state, lib.getName().c_str());
	if (!libTable.isTable())
	{
		lua::utils::error(state, "Loaded Lua Native Library {} not found.", lib.getName());
		return;
	}

	libTable.push();
	luabridge::push(state, luabridge::Nil());
	while (lua_next(state, -2))
	{
		LuaRef key = LuaRef::fromStack(state, -2);
		LuaRef val = LuaRef::fromStack(state, -1);

		libTable[key] = val;
		lua_pop(state, 1);
	}

	lua_pop(state, 1);
	libTable.pop();
}

void LuaLibraryManager::openCustomLibrary(lua_State* state, const std::shared_ptr<LuaRef>& env, const LuaLibrary& lib)
{
	if (env == nullptr)
	{
		lua::utils::error(state, "NULL Environment on Lua open library routine.");
		return;
	}

	const auto libname = customLibname(lib.getName());

	if (!lib.isLoaded())
	{
		auto globalNamespace = luabridge::getGlobalNamespace(state);
		auto libNamespace = globalNamespace.beginNamespace(libname.data());

		bool result = lib.getConstructor()(libNamespace, state);
		libNamespace.endNamespace();

		if (!result)
		{
			lua::utils::error(state, "Error during Lua Custom library {}.", libname);
			return;
		}
	}

	(*env)[libname] = luabridge::getGlobal(state, libname.c_str());
}

LuaRef LuaLibraryManager::LUA_import(const std::string& spath, lua_State* state)
{
	Path path(spath);
	if (!path.is_absolute())
	{
		LuaScript currentScript = LuaScriptManager::instance().getCurrentRunScript();
		if (!currentScript)
			return LuaRef(state);

		path = resources::absolute(currentScript.getDirectory() / path);
	}

	if (!path.has_extension())
		path += ".lua";

	LuaScript script = LuaScriptManager::instance().getScript(path.string());
	if (!script)
	{
		lua::utils::error(state, "Script file {} not found.", path.string());
		return LuaRef(state);
	}

	LuaRef env = LuaRef::newTable(state);
	script(env);

	return env;
}

void LuaLibraryManager::LUA_openlib(const std::string& name, lua_State* state)
{
	return instance().openLibrary(state, name);
}
