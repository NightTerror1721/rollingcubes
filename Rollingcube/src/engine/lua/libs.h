#pragma once

#include <unordered_set>
#include <unordered_map>

#include "utils/logger.h"

#include "environment.h"


using LuaLibraryConstructor = std::function<bool(luabridge::Namespace&, lua_State*)>;


class LuaLibrary
{
private:
	std::string _name;
	LuaLibraryConstructor _constructor;
	std::unordered_set<std::string> _dependences;

	mutable bool _built = false;

public:
	LuaLibrary() = delete;
	LuaLibrary(const LuaLibrary&) = delete;
	LuaLibrary(LuaLibrary&&) noexcept = delete;
	~LuaLibrary() = default;

	LuaLibrary& operator= (const LuaLibrary&) = delete;
	LuaLibrary& operator= (LuaLibrary&&) noexcept = delete;

	constexpr bool operator== (const LuaLibrary& other) const noexcept { return _name == other._name; }
	constexpr auto operator<=> (const LuaLibrary& other) const noexcept { return _name <=> other._name; }

public:
	inline LuaLibrary(const std::string name, const LuaLibraryConstructor& constructor, std::initializer_list<std::string> dependences) :
		_name(name),
		_constructor(constructor),
		_dependences(dependences)
	{}

public:
	constexpr const std::string& getName() const { return _name; }

	constexpr const LuaLibraryConstructor& getConstructor() const { return _constructor; }

	inline bool hasDependences() const { return !_dependences.empty(); }
	constexpr const std::unordered_set<std::string>& getDependences() const { return _dependences; }

	bool openToEnv(const LuaRef& env) const;
};









class LuaLibraryManager
{
private:
	static LuaLibraryManager Instance;

private:
	std::unordered_map<std::string, std::shared_ptr<LuaLibrary>> _libs = {};

public:
	LuaLibraryManager(const LuaLibraryManager&) = delete;
	LuaLibraryManager(LuaLibraryManager&&) noexcept = delete;

	LuaLibraryManager& operator= (const LuaLibraryManager&) = delete;
	LuaLibraryManager& operator= (LuaLibraryManager&&) noexcept = delete;

private:
	LuaLibraryManager() = default;
	~LuaLibraryManager() = default;

public:
	static inline LuaLibraryManager& instance() { return Instance; }

	inline bool exists(const std::string& name) const { return _libs.contains(name); }

	inline std::shared_ptr<LuaLibrary> get(const std::string& name) const
	{
		const auto& it = _libs.find(name);
		if (it == _libs.end())
			return nullptr;
		return it->second;
	}

	inline void registerLibrary(const std::string& name, const LuaLibraryConstructor& constructor, std::initializer_list<std::string> dependences = {})
	{
		if (_libs.contains(name))
		{
			logger::error("Lua Rollingcube library {} already registered.", name);
			return;
		}

		_libs.insert({ name, std::make_shared<LuaLibrary>(name, constructor, dependences) });
	}
};

inline LuaLibraryManager LuaLibraryManager::Instance = {};


inline bool LuaLibrary::openToEnv(const LuaRef& env) const
{
	if (hasDependences())
	{
		for (const auto& dep : _dependences)
		{
			auto depLib = LuaLibraryManager::instance().get(dep);
			if (depLib == nullptr)
			{
				lua::utils::error("Internal error with {} library on dependency load.", dep);
				return false;
			}
			depLib->openToEnv(env);
		}
	}

	if (!_built)
	{
		auto globalNamespace = lua::utils::getGlobalNamespace();
		auto libraryNamespace = globalNamespace.beginNamespace(_name.c_str());

		bool result = _constructor(libraryNamespace, lua::state());
		libraryNamespace.endNamespace();

		if (!result)
		{
			lua::utils::error("Error when opening Lua library {}.", _name);
			return false;
		}

		_built = true;
	}

	LuaRef libref = lua::utils::getGlobalValue(_name.c_str());
	if (!libref.isTable())
	{
		lua::utils::error("Internal unknown error with {} library.", _name);
		return false;
	}

	lua::utils::forEachInTable(libref, [&env](const LuaRef& key, const LuaRef& value) {
		env[key] = value;
		});

	return true;
}


#define defineLuaLibraryConstructor(_FunctionName, _RootNamespaceName, _StateParamName) \
bool _FunctionName(luabridge::Namespace& _RootNamespaceName, lua_State* _StateParamName)
