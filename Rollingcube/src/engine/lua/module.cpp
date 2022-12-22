#include "module.h"


LuaCallStack LuaCallStack::Instance = {};


LuaModule::LuaModule(const Path& path, LuaModule* parent) :
	_parent(parent),
	_env(),
	_path(normalize(path)),
	_directory(Path(_path).remove_filename()),
	_loadedLibs(),
	_includedModules(),
	_chunk(),
	_loaded(false)
{}

bool LuaModule::load()
{
	if (isLoaded())
		return false;

	if (_parent == nullptr)
	{
		_env = std::make_shared<LuaRef>(lua::utils::newTableRef());
		loadBuiltinElements();
	}
	else _env = _parent->_env;

	_loadedLibs = _parent == nullptr ? std::make_shared<std::unordered_set<std::string>>() : _parent->_loadedLibs;
	_includedModules = _parent == nullptr ? std::make_shared<std::unordered_set<Path>>() : _parent->_includedModules;

	lua_State* state = lua::state();
	int status = luaL_loadfile(state, _path.string().c_str());
	if (status != LUA_OK)
	{
		release();
		if (_parent != nullptr)
			lua::utils::error(lua::utils::extractErrorFromStack());
		else
			logger::error(lua::utils::extractErrorFromStack());
		return false;
	}

	_chunk = std::make_unique<LuaRef>(LuaRef::fromStack(state));
	_loaded = true;

	bool hasError = false;
	std::string errorMsg;
	_chunk->push();
	_env->push();
	lua_setupvalue(state, -2, 1);
	LuaCallStack::push(*this);
	try { lua::bridge::pcall(state); }
	catch (const LuaException& ex)
	{
		hasError = true;
		errorMsg = ex.what();
	}
	LuaCallStack::pop();

	if (hasError)
	{
		release();
		if (_parent != nullptr)
			lua::utils::error(errorMsg);
		else
			logger::error(errorMsg);
		return false;
	}

	_includedModules->insert(_path);

	return true;
}

void LuaModule::release()
{
	_loaded = false;
	_chunk.reset();
	_loadedLibs.reset();
	_includedModules.reset();
	_env.reset();
}

Path LuaModule::findRelativePath(const std::string& spath)
{
	Path path{ spath };
	if (!path.is_absolute())
		path = normalize(_directory / path);

	if (!path.has_extension())
		path += ".lua";

	return normalize(path);
}

Path LuaModule::findPathFromRoot(const std::string& spath)
{
	Path path{ spath };
	if (!path.is_absolute())
		path = normalize(resources::data / path);

	if (!path.has_extension())
		path += ".lua";

	return normalize(path);
}

bool LuaModule::includeModule(const std::string& spath, bool fromRoot)
{
	if (!isLoaded())
	{
		lua::utils::error("Cannot include sub-modules on unloaded module!");
		return false;
	}

	Path path = fromRoot ? findPathFromRoot(spath) : findRelativePath(spath);
	if (_includedModules->contains(path))
		return true;

	if (!LuaModule(path, this).load())
	{
		lua::utils::error("Error during loading of {} module!", path.string());
		return false;
	}

	return true;
}

bool LuaModule::openLibrary(const std::string& name)
{
	if (!isLoaded())
	{
		lua::utils::error("Cannot open libraries on unloaded module!");
		return false;
	}

	if (_loadedLibs->contains(name))
		return true;

	auto lib = LuaLibraryManager::instance().get(name);
	if (lib == nullptr)
	{
		lua::utils::error("Library {} does not exists!", name);
		return false;
	}

	if (!lib->openToEnv(*_env))
	{
		lua::utils::error("Internal error when opening {} library!", name);
		return false;
	}

	_loadedLibs->insert(name);

	return true;
}

void LuaModule::LUA_include(const std::string& path, LuaRef luaFromRoot)
{
	auto mod = getCurrentRunningModule();
	if (mod == nullptr)
	{
		lua::utils::error("'include' function can only use on global scope.");
		return;
	}

	bool fromRoot = luaFromRoot.cast<bool>().value();
	mod->includeModule(path, fromRoot);
}

void LuaModule::LUA_openlib(const std::string& name)
{
	auto mod = getCurrentRunningModule();
	if (mod == nullptr)
	{
		lua::utils::error("'openlib' function can only use on global scope.");
		return;
	}

	mod->openLibrary(name);
}

void LuaModule::loadBuiltinElements()
{
	static constinit bool initiated = false;
	if (!initiated)
	{
		lua::utils::getGlobalNamespace()
			.addFunction("include", *LUA_include)
			.addFunction("openlib", *LUA_openlib);
		initiated = true;
	}

	setValueFromGlobal(lua::constants::include);
	setValueFromGlobal(lua::constants::openlib);

	setValueFromGlobal(lua::constants::xpcall);
	setValueFromGlobal(lua::constants::select);
	setValueFromGlobal(lua::constants::next);
	setValueFromGlobal(lua::constants::base);
	setValueFromGlobal(lua::constants::load);
	setValueFromGlobal(lua::constants::getmetatable);
	setValueFromGlobal(lua::constants::tonumber);
	setValueFromGlobal(lua::constants::require);
	setValueFromGlobal(lua::constants::_VERSION);
	setValueFromGlobal(lua::constants::rawget);
	setValueFromGlobal(lua::constants::rawset);
	setValueFromGlobal(lua::constants::rawequal);
//	setValueFromGlobal(lua::constants::_G);
	setValueFromGlobal(lua::constants::rawlen);
	setValueFromGlobal(lua::constants::error);
	setValueFromGlobal(lua::constants::tostring);
	setValueFromGlobal(lua::constants::table);
	setValueFromGlobal(lua::constants::collectgarbage);
	setValueFromGlobal(lua::constants::warn);
	setValueFromGlobal(lua::constants::pcall);
	setValueFromGlobal(lua::constants::loadfile);
	setValueFromGlobal(lua::constants::ipairs);
	setValueFromGlobal(lua::constants::setmetatable);
	setValueFromGlobal(lua::constants::coroutine);
	setValueFromGlobal(lua::constants::type);
	setValueFromGlobal(lua::constants::assert);
	setValueFromGlobal(lua::constants::dofile);
	setValueFromGlobal(lua::constants::print);
	setValueFromGlobal(lua::constants::pairs);
}











LuaModuleManager LuaModuleManager::Instance = {};

std::shared_ptr<LuaModule> LuaModuleManager::load(const Path& rawpath)
{
	Path path = LuaModule::normalize(rawpath);
	if (_modules.contains(path))
	{
		auto mod = _modules.at(path);
		mod->reload();
		return mod;
	}

	std::shared_ptr<LuaModule> mod{ new LuaModule(path, nullptr) };
	if (!mod->load())
		return nullptr;

	_modules.insert({ std::move(path), mod });

	return mod;
}

std::shared_ptr<LuaModule> LuaModuleManager::get(const Path& path)
{
	auto it = _modules.find(LuaModule::normalize(path));
	if (it == _modules.end())
		return nullptr;
	return it->second;
}
