#pragma once

#include <unordered_map>
#include <string>

#include <functional>

#include "natives.h"


class LuaGlobalState;
class LuaScriptManager;
class LuaLibraryManager;

using LuaLibraryConstructor = std::function<bool (luabridge::Namespace, lua_State*)>;

class LuaLibrary
{
public:
	enum class NativeId
	{
		NonNative = 0,

		Base,
		Coroutine,
		Package,
		String,
		Utf8,
		Table,
		Math,
		Io,
		Os,
		Debug
	};

private:
	NativeId _nativeId = NativeId::NonNative;
	std::string _name = std::string("", 0);
	LuaLibraryConstructor _constructor = {};
	bool _loaded = false;

public:
	LuaLibrary() = default;
	LuaLibrary(const LuaLibrary&) = default;
	LuaLibrary(LuaLibrary&&) noexcept = default;
	~LuaLibrary() = default;

	LuaLibrary& operator= (const LuaLibrary&) = default;
	LuaLibrary& operator= (LuaLibrary&&) noexcept = default;

	constexpr bool operator== (const LuaLibrary& other) const noexcept
	{
		if (_nativeId == NativeId::NonNative)
			return other._nativeId == NativeId::NonNative && _name == other._name;
		return _nativeId == other._nativeId;
	}

	constexpr auto operator<=> (const LuaLibrary& other) const noexcept
	{
		if (_nativeId == NativeId::NonNative)
		{
			if(other._nativeId != NativeId::NonNative)
				return std::strong_ordering::greater;
			return _name <=> other._name;
		}

		if (other._nativeId == NativeId::NonNative)
			return std::strong_ordering::less;

		return _nativeId <=> other._nativeId;
	}

public:
	inline LuaLibrary(std::string_view name, const LuaLibraryConstructor& constructor) :
		_nativeId(NativeId::NonNative),
		_name(name.data()),
		_constructor(constructor),
		_loaded(false)
	{}

	constexpr bool isNative() const { return _nativeId != NativeId::NonNative; }
	constexpr NativeId getNativeId() const { return _nativeId; }

	constexpr const std::string& getName() const { return _name; }

	constexpr const LuaLibraryConstructor& getConstructor() const { return _constructor; }

private:
	inline LuaLibrary(NativeId nativeId, std::string_view name) :
		_nativeId(nativeId),
		_name(name),
		_constructor(),
		_loaded(false)
	{}

	inline bool isLoaded() const { return _loaded; }
	inline void setLoaded() { _loaded = true; }

private:
	static const LuaLibrary NativeBase;
	static const LuaLibrary NativeCoroutine;
	static const LuaLibrary NativePackage;
	static const LuaLibrary NativeString;
	static const LuaLibrary NativeUtf8;
	static const LuaLibrary NativeTable;
	static const LuaLibrary NativeMath;
	static const LuaLibrary NativeIo;
	static const LuaLibrary NativeOs;
	static const LuaLibrary NativeDebug;

public:
	static const LuaLibrary& getNative(NativeId nativeId)
	{
		using enum NativeId;
		switch (nativeId)
		{
			case Base: return NativeBase;
			case Coroutine: return NativeCoroutine;
			case Package: return NativePackage;
			case String: return NativeString;
			case Utf8: return NativeUtf8;
			case Table: return NativeTable;
			case Math: return NativeMath;
			case Io: return NativeIo;
			case Os: return NativeOs;
			case Debug: return NativeDebug;

			default:
				logger::warn("Invalid native library id {}.", static_cast<int>(nativeId));
				return NativeBase;
		}
	}

	friend LuaLibraryManager;
};



class LuaLibraryManager
{
private:
	static constexpr std::string_view GamelibPrefix = "Rollingcube.";

private:
	std::unordered_map<std::string, LuaLibrary> _libs;

public:
	LuaLibraryManager(const LuaLibraryManager&) = delete;
	LuaLibraryManager(LuaLibraryManager&&) noexcept = delete;
	~LuaLibraryManager() = default;

	LuaLibraryManager& operator= (const LuaLibraryManager&) = delete;
	LuaLibraryManager& operator= (LuaLibraryManager&&) noexcept = delete;

private:
	LuaLibraryManager();

public:
	void registerLibrary(std::string_view libraryName, const LuaLibraryConstructor& libraryConstructor);

private:
	void openLibrary(lua_State* state, const std::string& libname) const;

private:
	static void loadBuiltInData(lua_State* state);
	static void openLuaLibrary(lua_State* state, const std::shared_ptr<LuaRef>& env, const LuaLibrary& lib);
	static void openCustomLibrary(lua_State* state, const std::shared_ptr<LuaRef>& env, const LuaLibrary& lib);

private:
	static LuaRef LUA_import(const std::string& spath, lua_State* state);
	static void LUA_openlib(const std::string& name, lua_State* state);

private:
	static LuaLibraryManager Instance;

public:
	static inline LuaLibraryManager& instance() { return Instance; }

public:
	friend LuaScriptManager;
	friend LuaGlobalState;
};


#define defineLuaLibraryConstructor(_FunctionName, _StateParamName) bool _FunctionName(lua_State* _StateParamName)
