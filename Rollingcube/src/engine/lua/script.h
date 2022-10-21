#pragma once

#include <initializer_list>
#include <concepts>
#include <ranges>

#include "chunk.h"


class LuaScript
{
private:
	std::shared_ptr<LuaChunk> _chunk = nullptr;

public:
	constexpr LuaScript() = default;
	constexpr LuaScript(const LuaScript&) = default;
	constexpr LuaScript(LuaScript&&) noexcept = default;
	constexpr ~LuaScript() = default;

	constexpr LuaScript& operator= (const LuaScript&) = default;
	constexpr LuaScript& operator= (LuaScript&&) noexcept = default;

public:
	inline LuaScript(const std::shared_ptr<LuaChunk>& chunk) : _chunk(chunk) {}

	inline bool isValid() const { return _chunk != nullptr && _chunk->isValid(); }

	inline operator bool() const { return isValid(); }
	inline bool operator! () const { return !isValid(); }

	inline Path getPath() const { return isValid() ? _chunk->getPath() : Path(); }
	inline Path getDirectory() const { return isValid() ? _chunk->getDirectory() : Path(); }

	inline const std::shared_ptr<LuaRef>& getEnv() { return _chunk->getEnv(); }
	inline const std::shared_ptr<const LuaRef>& getEnv() const { return _chunk->getEnv(); }

	template <typename _Ty = LuaRef>
	inline _Ty getEnvValue(std::string_view name) const
	{
		if constexpr (std::same_as<LuaRef, _Ty>)
			return (*getEnv())[name];
		else
			return (*getEnv())[name].cast<_Ty>();
	}

	template <typename _Ty>
	inline void setEnvValue(std::string_view name, const _Ty& value)
	{
		(*getEnv())[name] = value;
	}

	inline void setEnvValueFromGlobal(std::string_view globalName)
	{
		if (isValid())
			setEnvValue(globalName, luabridge::getGlobal(_chunk->getLuaState(), globalName.data()));
	}

private:
	template <typename _Ty> requires
		std::ranges::range<_Ty> &&
		(
			std::convertible_to<std::ranges::range_value_t<_Ty>, const char*> ||
			std::convertible_to<std::ranges::range_value_t<_Ty>, std::string> ||
			std::convertible_to<std::ranges::range_value_t<_Ty>, std::string_view>
		)
	void _setEnvValuesFromGlobal(const _Ty& names) const
	{
		if (isValid())
		{
			const LuaRef& env = *_chunk->getEnv();
			lua_State* state = _chunk->getLuaState();

			const auto end = std::ranges::end(names);
			for (auto it = std::ranges::begin(names); it != end; ++it)
			{
				const char* name;
				if constexpr (std::convertible_to<std::ranges::range_value_t<_Ty>, std::string>)
					name = it->c_str();
				else if constexpr (std::convertible_to<std::ranges::range_value_t<_Ty>, std::string_view>)
					name = it->data();
				else
					name = static_cast<const char*>(*it);
				env[name] = luabridge::getGlobal(state, name);
			}
		}
	}

public:
	template <typename _Ty> requires
		std::ranges::range<_Ty> &&
		(
			std::convertible_to<std::ranges::range_value_t<_Ty>, const char*> ||
			std::convertible_to<std::ranges::range_value_t<_Ty>, std::string> ||
			std::convertible_to<std::ranges::range_value_t<_Ty>, std::string_view>
		)
	inline void setEnvValuesFromGlobal(const _Ty& names) const
	{
		_setEnvValuesFromGlobal<_Ty>(names);
	}

	inline void setEnvValuesFromGlobal(std::initializer_list<std::string_view> names) const
	{
		_setEnvValuesFromGlobal(names);
	}

	template <std::convertible_to<std::string_view>... _ArgsTys>
	inline void setEnvValuesFromGlobal(_ArgsTys&&... names)
	{
		setEnvValuesFromGlobal({ std::forward<_ArgsTys>(names)... });
	}

public:
	inline void operator() () const
	{
		if (_chunk != nullptr)
			_chunk->run();
	}

	inline void operator() (const LuaRef& customEnv) const
	{
		if (_chunk != nullptr)
			_chunk->run(std::addressof(customEnv));
	}

	inline void operator() (const LuaRef* customEnv) const
	{
		if (_chunk != nullptr)
			_chunk->run(customEnv);
	}

	inline void operator() (const std::shared_ptr<LuaRef>& customEnv) const
	{
		if (_chunk != nullptr)
			_chunk->run(customEnv.get());
	}

	inline void operator() (const std::unique_ptr<LuaRef>& customEnv) const
	{
		if (_chunk != nullptr)
			_chunk->run(customEnv.get());
	}

public:
	class EnvPair
	{
	private:
		LuaScript& _scr;
		std::string_view const _name;

		inline EnvPair(LuaScript& scr, std::string_view name) : _scr(scr), _name(name) {}

	public:
		template <typename _Ty>
		inline const _Ty& operator= (const _Ty& value) { return _scr.setEnvValue(_name, value), value; }

		template <typename _Ty>
		inline _Ty& operator= (_Ty& value) { return _scr.setEnvValue(_name, value), value; }

		template <typename _Ty = LuaRef>
		inline operator _Ty() const { return _scr.getEnvValue<_Ty>(_name); }

		friend LuaScript;
	};

	class ConstEnvPair
	{
	private:
		const LuaScript& _scr;
		std::string_view const _name;

		inline ConstEnvPair(const LuaScript& scr, std::string_view name) : _scr(scr), _name(name) {}

	public:
		template <typename _Ty = LuaRef>
		inline operator _Ty() const { return _scr.getEnvValue<_Ty>(_name); }

		friend LuaScript;
	};

	inline EnvPair operator[] (std::string_view name) { return { *this, name }; }
	inline ConstEnvPair operator[] (std::string_view name) const { return { *this, name }; }
};
