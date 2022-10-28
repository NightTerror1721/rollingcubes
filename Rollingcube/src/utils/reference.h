#pragma once

#include <concepts>


template <typename _Ty>
class NullableReference
{
public:
	using ValueType = _Ty;
	using Reference = _Ty&;
	using Pointer = _Ty*;

private:
	Pointer _ptr = nullptr;

public:
	constexpr NullableReference() = default;
	constexpr NullableReference(const NullableReference&) = default;
	constexpr NullableReference(NullableReference&&) noexcept = default;
	constexpr ~NullableReference() = default;

	constexpr NullableReference& operator= (const NullableReference&) = default;
	constexpr NullableReference& operator= (NullableReference&&) noexcept = default;

	constexpr bool operator== (const NullableReference&) const noexcept = default;

	constexpr NullableReference(decltype(nullptr)) : NullableReference() {}

	constexpr NullableReference(const NullableReference<std::remove_const_t<ValueType>>& ref) requires std::is_const_v<ValueType>
		: _ptr(ref._ptr)
	{}

	constexpr NullableReference(Pointer ptr) : _ptr(ptr) {}

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

	constexpr Pointer operator& () const { return _ptr; }
};


template <typename _Ty>
using Reference = NullableReference<_Ty>;

template <typename _Ty>
using ConstReference = Reference<const _Ty>;
