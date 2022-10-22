#include "libs.h"

#include "constants.h"
#include "manager.h"

#include "math/glm.h"



namespace lua
{
	void initCustomLibs()
	{
		glm::lua::registerGlmToLua();
	}
}





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
	const LuaLibrary& lib = LuaLibrary::getNative(id);
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

LuaLibraryManager::~LuaLibraryManager()
{
	LuaScriptManager::instance().clear();
}

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
		.addFunction(lua::lib::constants::import, &LUA_import)
		.addFunction(lua::lib::constants::openlib, &LUA_openlib);
}

void LuaLibraryManager::openLibrary(lua_State* state, const std::string& libname) const
{
	const LuaRef* env = LuaScriptManager::instance().getCurrentRunScriptEnv();
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
		openLuaLibrary(state, env, lib);
	else
		openCustomLibrary(state, env, lib);
}

void LuaLibraryManager::openLuaLibrary(lua_State* state, const LuaRef* env, const LuaLibrary& lib)
{
	if (env == nullptr)
	{
		lua::utils::error(state, "NULL Environment on Lua open library routine.");
		return;
	}

	if (lib.getNativeId() == LuaLibrary::NativeId::Base)
		return openLuaBaseLibrary(state, env);

	const char* libname = nullptr;
	switch (lib.getNativeId())
	{
		using enum LuaLibrary::NativeId;

		case Coroutine: libname = lua::lib::constants::coroutine; break;
		case Package: libname = lua::lib::constants::package; break;
		case String: libname = lua::lib::constants::string; break;
		case Utf8: libname = lua::lib::constants::utf8; break;
		case Table: libname = lua::lib::constants::table; break;
		case Math: libname = lua::lib::constants::math; break;
		case Io: libname = lua::lib::constants::io; break;
		case Os: libname = lua::lib::constants::os; break;
		case Debug: libname = lua::lib::constants::debug; break;
	}

	if (libname == nullptr)
	{
		lua::utils::error(state, "Cannot open Lua Library {}: Unknown Lua Native Library ID {}.", lib.getName(), static_cast<int>(lib.getNativeId()));
		return;
	}

	(*env)[libname] = luabridge::getGlobal(state, libname);
}

void LuaLibraryManager::openLuaBaseLibrary(lua_State* state, const LuaRef* env)
{
#define loadBaseElement(_Name) (*env)[lua::lib::constants::_Name] = luabridge::getGlobal(state, lua::lib::constants::_Name)

	loadBaseElement(xpcall);
	loadBaseElement(select);
	loadBaseElement(next);
	loadBaseElement(base);
	loadBaseElement(load);
	loadBaseElement(getmetatable);
	loadBaseElement(tonumber);
	loadBaseElement(require);
	loadBaseElement(_VERSION);
	loadBaseElement(rawget);
	loadBaseElement(rawset);
	loadBaseElement(rawequal);
	loadBaseElement(_G);
	loadBaseElement(rawlen);
	loadBaseElement(error);
	loadBaseElement(tostring);
	loadBaseElement(table);
	loadBaseElement(collectgarbage);
	loadBaseElement(warn);
	loadBaseElement(pcall);
	loadBaseElement(loadfile);
	loadBaseElement(ipairs);
	loadBaseElement(setmetatable);
	loadBaseElement(coroutine);
	loadBaseElement(type);
	/* loadBaseElement(assert); */ (*env)[lua::lib::constants::assert] = luabridge::getGlobal(state, lua::lib::constants::assert);
	loadBaseElement(dofile);
	loadBaseElement(print);
	loadBaseElement(pairs);

#undef loadBaseElement
}

void LuaLibraryManager::openCustomLibrary(lua_State* state, const LuaRef* env, const LuaLibrary& lib)
{
	if (env == nullptr)
	{
		lua::utils::error(state, "NULL Environment on Lua open library routine.");
		return;
	}

	const auto libname = lib.getName();

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

		lib.setLoaded();
	}

	LuaRef libref = luabridge::getGlobal(state, libname.c_str());
	if (!libref.isTable())
	{
		lua::utils::error(state, "Malformed Lua Custom library {}.", libname);
		return;
	}

	libref.push();
	luabridge::push(state, luabridge::Nil());
	while (lua_next(state, -2))
	{
		LuaRef key = LuaRef::fromStack(state, -2);
		LuaRef val = LuaRef::fromStack(state, -1);

		(*env)[key] = val;

		lua_pop(state, 1);
	}

	lua_pop(state, 1);
	libref.pop();
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
