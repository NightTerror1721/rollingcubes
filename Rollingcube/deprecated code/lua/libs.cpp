#include "libs.h"

#include "constants.h"
#include "manager.h"

#include "math/glm.h"
#include "engine/entities.h"
#include "game/theme.h"



namespace lua
{
	void initCustomLibs()
	{
		static constinit bool initiatedFlag = false;

		if (!initiatedFlag)
		{
			lua::lib::registerGlmToLua();
			lua::lib::registerCameraLibToLua();
			lua::lib::registerShaderLibToLua();
			lua::lib::registerEntitiesLibToLua();
			lua::lib::registerThemesLibToLua();
			lua::lib::registerTilesLibToLua();
			lua::lib::registerBlocksLibToLua();
			lua::lib::registerModelsLibToLua();
			lua::lib::registerBallsLibToLua();

			initiatedFlag = true;
		}
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

void LuaLibraryManager::registerLibrary(std::string_view libraryName, const LuaLibraryConstructor& libraryConstructor, std::initializer_list<std::string> dependences)
{
	std::string name = customLibname(libraryName);
	if (_libs.contains(name))
	{
		logger::error("Lua Rollingcube library {} already registered.", libraryName);
		return;
	}

	_libs.insert({ name, LuaLibrary(name, libraryConstructor, dependences) });
	logger::info("Lua Rollingcube library {} registered correctly!", name);
}

void LuaLibraryManager::loadBuiltInData(lua_State* state)
{
	luabridge::getGlobalNamespace(state)
		.addFunction(lua::lib::constants::import, &LUA_import)
		.addFunction(lua::lib::constants::include, &LUA_include)
		.addFunction(lua::lib::constants::openlib, &LUA_openlib);
}

void LuaLibraryManager::openLibrary(lua_State* state, const std::string& libname) const
{
	const LuaEnv* env = LuaScriptManager::instance().getCurrentRunScriptEnv();
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

void LuaLibraryManager::openLuaLibrary(lua_State* state, const LuaEnv* env, const LuaLibrary& lib)
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

void LuaLibraryManager::openLuaBaseLibrary(lua_State* state, const LuaEnv* env)
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
//	loadBaseElement(_G);
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

void LuaLibraryManager::openCustomLibrary(lua_State* state, const LuaEnv* env, const LuaLibrary& lib) const
{
	if (env == nullptr)
	{
		lua::utils::error(state, "NULL Environment on Lua open library routine.");
		return;
	}

	// initiate custom libs //
	lua::initCustomLibs();

	const auto libname = lib.getName();

	LuaRef openedLibs = getOpenedLibsTable(state, env);
	if (!openedLibs[libname].isNil()) // is opened? //
		return;

	openedLibs[libname] = true;

	if (lib.hasDependences())
	{
		for (const auto& dep : lib.getDependences())
			openLibrary(state, customLibname(dep));
	}

	if (!lib.isBuilt())
	{
		auto globalNamespace = luabridge::getGlobalNamespace(state);
		auto libNamespace = globalNamespace.beginNamespace(libname.data());

		bool result = lib.getConstructor()(libNamespace, state);
		libNamespace.endNamespace();

		if (!result)
		{
			openedLibs[libname] = luabridge::Nil();
			lua::utils::error(state, "Error during Lua Custom library {}.", libname);
			return;
		}

		lib.setBuilt();
	}

	LuaRef libref = luabridge::getGlobal(state, libname.c_str());
	if (!libref.isTable())
	{
		openedLibs[libname] = luabridge::Nil();
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
}

bool LuaLibraryManager::importScript(lua_State* state, const std::string& spath)
{
	Path path = findRelativePath(spath);
	if (path.empty())
	{
		lua::utils::error(state, "Script file {} not found.", spath);
		return false;
	}

	LuaScript script;
	auto it = _importedScripts.find(path);
	if (it != _importedScripts.end())
		script = it->second;
	else
	{
		script = LuaScriptManager::instance().getScript(path.string());
		if (!script)
		{
			lua::utils::error(state, "Error on load {} script file.", spath);
			return false;
		}

		script();
	}

	const LuaEnv* env = LuaScriptManager::instance().getCurrentRunScriptEnv();
	if (env == nullptr)
	{
		lua::utils::error(state, "NULL Environment on Lua open library routine.");
		return false;
	}

	script.getEnv().push();
	luabridge::push(state, luabridge::Nil());
	while (lua_next(state, -2))
	{
		LuaRef key = LuaRef::fromStack(state, -2);
		LuaRef val = LuaRef::fromStack(state, -1);

		(*env)[key] = val;

		lua_pop(state, 1);
	}

	lua_pop(state, 1);

	return true;
}

LuaRef LuaLibraryManager::getOpenedLibsTable(lua_State* state, const LuaEnv* env)
{
	LuaRef openedLibs = (*env)[CustomLibsOpened];
	if (!openedLibs.isTable())
	{
		(*env)[CustomLibsOpened] = LuaRef::newTable(state);
		openedLibs = (*env)[CustomLibsOpened];
	}
	return std::move(openedLibs);
}

Path LuaLibraryManager::findRelativePath(const std::string& spath)
{
	Path path(spath);
	if (!path.is_absolute())
	{
		LuaScript currentScript = LuaScriptManager::instance().getCurrentRunScript();
		if (!currentScript)
			return Path();

		path = resources::absolute(currentScript.getDirectory() / path);
	}

	if (!path.has_extension())
		path += ".lua";

	return std::filesystem::weakly_canonical(path);
}

LuaScript LuaLibraryManager::findRelativeScript(const std::string& spath)
{
	return LuaScriptManager::instance().getScript(findRelativePath(spath).string());
}

bool LuaLibraryManager::LUA_import(const std::string& spath, lua_State* state)
{
	return instance().importScript(state, spath);
}

bool LuaLibraryManager::LUA_include(const std::string& spath, lua_State* state)
{
	Path path = findRelativePath(spath);
	if(path.empty())
	{
		lua::utils::error(state, "Script file {} not found.", spath);
		return false;
	}

	return LuaScriptManager::instance().getCurrentRunScript().includeSubScript(path);
}

void LuaLibraryManager::LUA_openlib(const std::string& name, lua_State* state)
{
	return instance().openLibrary(state, name);
}
