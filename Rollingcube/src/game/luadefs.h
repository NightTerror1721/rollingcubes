#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <optional>
#include <concepts>

#include "engine/lua/lua.h"
#include "utils/resources.h"
#include "utils/reference.h"
#include "utils/manager.h"


enum class LuaModelType
{
	Unknown = 0,

	Block,
	Item,
	Model,
	Tile,
	Theme,
	Ball,
	Mob,
	Menu
};

namespace std
{
	template <>
	struct hash<LuaModelType> : public hash<int>
	{
		inline std::size_t operator() (const LuaModelType& keyVal) const noexcept
		{
			return hash<int>::operator() (static_cast<int>(keyVal));
		}
	};
}




class LuaModel
{
public:
	using Type = LuaModelType;
	static constexpr std::string_view getModelTypeName(Type type)
	{
		switch (type)
		{
			case Type::Block: return "blocks";
			case Type::Item: return "items";
			case Type::Model: return "models";
			case Type::Tile: return "tiles";
			case Type::Theme: return "themes";
			case Type::Ball: return "balls";
			case Type::Mob: return "mobs";
			case Type::Menu: return "menus";

			default: return "<unknown-model-type>";
		}
	}

	static constexpr std::size_t TypeCount = static_cast<std::size_t>(static_cast<int>(Type::Menu) + 1);

public:
	using Id = unsigned int;

protected:
	static constexpr std::string_view FunctionOnInit = "OnInit";
	static constexpr std::string_view FunctionOnDestroy = "OnDestroy";

protected:
	Id _id = 0;
	std::string _name = {};
	LuaScript _script;
	bool _loaded = false;

private:
	mutable std::unordered_map<std::string, std::unique_ptr<LuaRef>> _luaCache;

public:
	LuaModel() = default;
	LuaModel(const LuaModel&) = delete;
	LuaModel(LuaModel&&) noexcept = default;

	LuaModel& operator= (const LuaModel&) = delete;
	LuaModel& operator= (LuaModel&&) noexcept = default;

	constexpr bool operator== (const LuaModel& right) const noexcept { return _id == right._id; }
	constexpr auto operator<=> (const LuaModel& right) const noexcept { return _id <=> right._id; }

public:
	constexpr Id getId() const { return _id; }

	constexpr void setName(const std::string& name) { _name = name; }
	constexpr const std::string& getName() const { return _name; }

	constexpr bool isLoaded() const { return _loaded; }

	virtual Type getType() const = 0;

public:
	virtual ~LuaModel();

	virtual bool load();

	void reload();

protected:
	virtual void clear();

	std::optional<Path> findModelFile() const;

private:
	void init();

protected:
	Reference<LuaRef> findLuaObject(std::string_view name) const;

	template <typename... _ArgsTys>
	inline void vcall(std::string_view name, _ArgsTys&&... args)
	{
		auto fn = findLuaObject(name);
		if (fn != nullptr)
		{
			try
			{
				(*fn)(std::forward<_ArgsTys>(args)...);
			}
			catch (const LuaException& ex)
			{
				logger::error("Lua function {} call error: {}", name, ex.what());
			}
		}
	}

	template <typename _RetTy, typename... _ArgsTys>
	inline _RetTy call(std::string_view name, _ArgsTys&&... args)
	{
		auto fn = findLuaObject(name);
		if (fn != nullptr)
		{
			try
			{
				if constexpr (std::same_as<LuaRef, _RetTy>)
					return (*fn)(std::forward<_ArgsTys>(args)...);
				else
					return (*fn)(std::forward<_ArgsTys>(args)...).cast<_RetTy>();
			}
			catch (const LuaException& ex)
			{
				logger::error("Lua function {} call error: {}", name, ex.what());
			}
		}
		else
		{
			if constexpr (std::same_as<LuaRef, _RetTy>)
				return LuaRef(luabridge::Nil());
			else
				return LuaRef(luabridge::Nil()).cast<_RetTy>();
		}
	}
};




template <std::derived_from<LuaModel> _LuaModelTy>
class LuaModelManager : public Manager<_LuaModelTy, std::string>
{
private:
	using ManagerType = Manager<_LuaModelTy>;

protected:
	inline LuaModelManager() : ManagerType(nullptr) {}

public:
	virtual ManagerReference<_LuaModelTy> load(const std::string& name)
	{
		ManagerReference<_LuaModelTy> ref = this->get(name);
		if (ref != nullptr)
		{
			ref->reload();
			return ref;
		}

		ref = this->emplace<_LuaModelTy>(name);
		if (ref == nullptr)
		{
			logger::error("Cannot load '{}' Model!", name);
			return nullptr;
		}

		ref->setName(name);
		if (!ref->load())
		{
			this->destroy(name);
			return nullptr;
		}

		return ref;
	}
};
