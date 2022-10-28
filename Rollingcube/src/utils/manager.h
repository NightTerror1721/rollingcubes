#pragma once

#include <concepts>
#include <string>
#include <unordered_map>
#include <memory>

#include "reference.h"
#include "optref.h"


template <typename _Ty, typename _IdTy = std::string> requires
	std::default_initializable<_Ty>&&
	std::destructible<_Ty>
class Manager;


template <typename _Ty>
using ManagerReference = Reference<_Ty>;

template <typename _Ty>
using ConstManagerReference = ConstReference<_Ty>;


template <typename _Ty, typename _IdTy> requires
	std::default_initializable<_Ty> &&
	std::destructible<_Ty>
class Manager
{
public:
	using ValueType = _Ty;
	using Reference = ManagerReference<ValueType>;
	using ConstReference = ConstManagerReference<ValueType>;

	using IdType = _IdTy;

private:
	Manager* _parent;
	std::unordered_map<IdType, std::unique_ptr<ValueType>> _cache;

protected:
	inline Manager(Manager* parent) :
		_parent(parent), _cache()
	{}

	Manager(const Manager&) = delete;
	Manager& operator= (const Manager&) = delete;

public:
	~Manager() = default;

	Manager(Manager&&) noexcept = default;
	Manager& operator= (Manager&&) noexcept = default;

	inline const std::shared_ptr<const Manager>& parent() const { return _parent; }
	inline bool hasParent() const { return _parent != nullptr; }

	inline bool contains(const IdType& id) const
	{
		if (_cache.contains(id))
			return true;

		return hasParent() && _parent->contains(id);
	}

	inline Reference get(const IdType& id)
	{
		auto it = _cache.find(id);
		if (it != _cache.end())
			return it->second.get();

		if (!hasParent())
			return {};
		return _parent->get(id);
	}

	inline ConstReference get(const IdType& id) const
	{
		auto it = _cache.find(id);
		if (it != _cache.end())
			return it->second.get();

		if (!hasParent())
			return {};
		return const_cast<const Manager*>(_parent)->get(id);
	}

	inline void destroy(const IdType& id)
	{
		_cache.erase(id);
	}

	virtual inline void clear() { _cache.clear(); }


protected:
	template <typename _ObjTy = _Ty, typename... _ArgTys> requires
		std::derived_from<_ObjTy, _Ty> &&
		std::constructible_from<_ObjTy, _ArgTys...>
	inline Reference emplace(const IdType& id, _ArgTys&&... args)
	{
		auto res = _cache.insert({ id, std::make_unique<_ObjTy>(std::forward<_ArgTys>(args)...) });
		if (!res.second)
			return {};
		return res.first->second.get();
	}
};

