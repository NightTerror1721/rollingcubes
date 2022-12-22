#pragma once

#include <stack>

#include "utils/resources.h"
#include "utils/reference.h"

#include "environment.h"
#include "libs.h"
#include "constants.h"
#include "localvalues.h"


class LuaModule;
class LuaModuleManager;

class LuaCallStack
{
public:
	friend LuaModule;

private:
	static LuaCallStack Instance;

private:
	std::stack<LuaModule*> _stack = {};

public:
	LuaCallStack(const LuaCallStack&) = delete;
	LuaCallStack(LuaCallStack&&) noexcept = delete;

	LuaCallStack& operator= (const LuaCallStack&) = delete;
	LuaCallStack& operator= (LuaCallStack&&) noexcept = delete;

private:
	LuaCallStack() = default;
	~LuaCallStack() = default;

private:
	static inline void push(LuaModule& mod) { Instance._stack.push(std::addressof(mod)); }
	static inline void pop() { Instance._stack.pop(); }
	static inline Reference<LuaModule> top() { return Instance._stack.empty() ? nullptr : Instance._stack.top(); }
};

class LuaModule
{
public:
	friend LuaModuleManager;

private:
	LuaModule* _parent;
	std::shared_ptr<LuaRef> _env;

	Path _path;
	Path _directory;
	
	std::shared_ptr<std::unordered_set<std::string>> _loadedLibs;
	std::shared_ptr<std::unordered_set<Path>> _includedModules;

	std::unique_ptr<LuaRef> _chunk;
	bool _loaded;

public:
	LuaModule(const LuaModule&) = delete;
	LuaModule(LuaModule&&) noexcept = delete;

	LuaModule& operator= (const LuaModule&) = delete;
	LuaModule& operator= (LuaModule&&) noexcept = delete;

private:
	LuaModule(const Path& path, LuaModule* parent);

	Path findRelativePath(const std::string& path);
	Path findPathFromRoot(const std::string& path);
	void loadBuiltinElements();

	bool includeModule(const std::string& path, bool fromRoot);
	bool openLibrary(const std::string& name);

public:
	inline ~LuaModule() { release(); };

public:
	bool load();
	void release();

public:
	constexpr bool isLoaded() const { return _loaded; }

	inline void reload() { release(), load(); }

	template <typename _Ty> requires LuaValidValue<_Ty> || std::same_as<_Ty, LuaRef>
	inline void setValue(std::string_view name, const _Ty& value) const { (*_env)[name] = value; }

	inline void setNil(std::string_view name) const { (*_env)[name] = nullptr; }

	template <typename _Ty> requires LuaValidValue<_Ty> || std::same_as<_Ty, LuaRef>
	inline _Ty getValue(std::string_view name) const { return (*_env)[name].cast<_Ty>(); }

	template <>
	inline LuaRef getValue<LuaRef>(std::string_view name) const { return (*_env)[name]; }

	inline void setValueFromGlobal(std::string_view name) const { lua::utils::setFromGlobal(name, *_env); }

private:
	static void LUA_include(const std::string& path, LuaRef fromRoot);
	static void LUA_openlib(const std::string& name);

private:
	static inline Path normalize(const Path& path) { return std::filesystem::absolute(std::filesystem::weakly_canonical(path)); }

public:
	static inline Reference<LuaModule> getCurrentRunningModule() { return LuaCallStack::top(); }
};









class LuaModuleManager
{
private:
	static LuaModuleManager Instance;

private:
	std::unordered_map<Path, std::shared_ptr<LuaModule>> _modules = {};
	
public:
	LuaModuleManager(const LuaModuleManager&) = delete;
	LuaModuleManager(LuaModuleManager&&) noexcept = delete;

	LuaModuleManager& operator= (const LuaModuleManager&) = delete;
	LuaModuleManager& operator= (LuaModuleManager&&) noexcept = delete;

private:
	LuaModuleManager() = default;
	~LuaModuleManager() = default;

public:
	std::shared_ptr<LuaModule> load(const Path& path);

	std::shared_ptr<LuaModule> get(const Path& path);

public:
	static constexpr LuaModuleManager& instance() { return Instance; }

	inline bool exists(const Path& path) { return _modules.contains(LuaModule::normalize(path)); }

	inline void clear() { _modules.clear(); }
};
