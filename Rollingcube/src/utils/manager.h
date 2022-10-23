#pragma once

#include <concepts>
#include <string>
#include <unordered_map>
#include <memory>

#include "optref.h"


template <typename _Ty, typename _IdTy = std::string> requires
	std::default_initializable<_Ty>&&
	std::destructible<_Ty>
class Manager;


template <typename _Ty>
class ManagerReference
{
public:
	using ValueType = _Ty;
	using Reference = _Ty&;
	using Pointer = _Ty*;

	friend Manager;

private:
	Pointer _ptr = nullptr;

public:
	constexpr ManagerReference() = default;
	constexpr ManagerReference(const ManagerReference&) = default;
	constexpr ManagerReference(ManagerReference&&) noexcept = default;
	constexpr ~ManagerReference() = default;

	constexpr ManagerReference& operator= (const ManagerReference&) = default;
	constexpr ManagerReference& operator= (ManagerReference&&) noexcept = default;

	constexpr bool operator== (const ManagerReference&) const noexcept = default;

	constexpr ManagerReference(decltype(nullptr)) : ManagerReference() {}

	constexpr ManagerReference(const ManagerReference<std::remove_const_t<ValueType>>& ref) requires std::is_const_v<ValueType>
		: _ptr(ref._ptr)
	{}

	constexpr bool operator== (decltype(nullptr)) const noexcept { return _ptr == nullptr; }
	constexpr bool operator!= (decltype(nullptr)) const noexcept { return _ptr != nullptr; }

	constexpr operator bool() const { return _ptr != nullptr; }
	constexpr bool operator! () const { return _ptr == nullptr; }

	constexpr Reference operator* () const { return *_ptr; }

	constexpr Pointer operator-> () const { return _ptr; }

	template <typename _ArgTy> requires requires(ValueType& value, const _ArgTy& arg) { { value[arg] }; }
	constexpr decltype(auto) operator[] (const _ArgTy& arg) { return (*_ptr)[arg]; }

	template <typename _ArgTy> requires requires(const ValueType& value, const _ArgTy& arg) { { value[arg] }; }
	constexpr decltype(auto) operator[] (const _ArgTy& arg) const { return (*_ptr)[arg]; }

private:
	constexpr ManagerReference(Pointer ptr) : _ptr(ptr) {}
};

template <typename _Ty>
using ConstManagerReference = ManagerReference<const _Ty>;


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

