#pragma once

#include <memory>
#include <concepts>
#include <functional>

#include "manager.h"


class LuaLocalVariablesContainer
{
private:
	mutable std::unique_ptr<LuaRef> _vars = nullptr;

public:
	LuaLocalVariablesContainer() = default;
	LuaLocalVariablesContainer(const LuaLocalVariablesContainer&) = delete;
	LuaLocalVariablesContainer(LuaLocalVariablesContainer&&) noexcept = default;
	~LuaLocalVariablesContainer() = default;

	LuaLocalVariablesContainer& operator= (const LuaLocalVariablesContainer&) = delete;
	LuaLocalVariablesContainer& operator= (LuaLocalVariablesContainer&&) noexcept = default;

public:
	inline void storeLuaVariableFromState(const char* name, const LuaRef& value) const
	{
		if (_vars == nullptr)
			buildLuaLocalVariablesContainer();

		(*_vars)[name] = value;
	}

	inline LuaRef loadLuaVariableToState(const char* name) const
	{
		if (_vars == nullptr)
			buildLuaLocalVariablesContainer();

		return (*_vars)[name];
	}

	inline void clearLuaVariables()
	{
		_vars.reset();
	}

private:
	inline void buildLuaLocalVariablesContainer() const
	{
		if (_vars != nullptr)
			logger::warn("Attempt to build already built LuaLocalVariablesContainer");
		else
			_vars = std::make_unique<LuaRef>(LuaScriptManager::instance().getLuaState());
	}
};






namespace lua::lib::utils
{
	template <std::derived_from<LuaLocalVariablesContainer> _Ty>
	luabridge::Namespace::Class<_Ty>& addLuaLocalVariablesToClass(luabridge::Namespace::Class<_Ty>& clss)
	{
		const auto loadFn = [](const _Ty* self, const char* name) -> LuaRef { return self->loadLuaVariableToState(name); };
		const auto storeFn = [](const _Ty* self, const char* name, LuaRef value) -> void { self->storeLuaVariableFromState(name, value); };

		return clss
			.addFunction("getLocalVariable", std::function(loadFn))
			.addFunction("setLocalVariable", std::function(storeFn));
	}
}
