#pragma once

#include "environment.h"


class LocalValuesContainer
{
private:
	mutable std::unique_ptr<LuaRef> _locals = nullptr;

public:
	LocalValuesContainer() = default;
	LocalValuesContainer(const LocalValuesContainer&) = default;
	LocalValuesContainer(LocalValuesContainer&&) noexcept = default;
	~LocalValuesContainer() = default;

	LocalValuesContainer& operator= (const LocalValuesContainer&) = default;
	LocalValuesContainer& operator= (LocalValuesContainer&&) noexcept = default;

private:
	inline const LuaRef& locals() const
	{
		if (_locals == nullptr)
			_locals = std::make_unique<LuaRef>(lua::utils::newTableRef());
		return *_locals;
	}

public:
	template <LuaValidValue _Ty>
	inline void setLocalValue(const std::string& name, const _Ty& value)
	{
		locals()[name] = value;
	}

	template <LuaValidValue _Ty>
	inline _Ty getLocalValue(const std::string& name) const
	{
		return locals()[name].cast<_Ty>();
	}

	template <>
	inline LuaRef getLocalValue<LuaRef>(const std::string& name) const
	{
		return locals()[name];
	}

	inline void deleteLocalValue(const std::string& name) { setLocalValue(name, lua::bridge::Nil()); }

	inline void clearLocalValues() { _locals.reset(); }
};


namespace lua::lib
{
	template <std::derived_from<LocalValuesContainer> _Ty>
	inline bridge::Namespace::Class<_Ty>& addLocalValuesContainerToClass(bridge::Namespace::Class<_Ty>& clss)
	{
		const auto getFn = [](const _Ty* self, const std::string& name) -> LuaRef { return self->getLocalValue<LuaRef>(name); };
		const auto setFn = [](_Ty* self, const std::string& name, LuaRef value) -> void { self->setLocalValue(name, value); };
		const auto delFn = [](_Ty* self, const std::string& name) -> void { self->deleteLocalValue(name); };

		return clss
			.addFunction("getLocalValue", std::function(getFn))
			.addFunction("setLocalValue", std::function(setFn))
			.addFunction("deleteLocalValue", std::function(delFn));
	}
}
