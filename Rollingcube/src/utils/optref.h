#pragma once

#include <utility>

template <typename _Ty>
class optref
{
public:
	using value_type = _Ty;
	using reference_type = value_type&;
	using pointer_type = value_type*;

	static constexpr pointer_type absent_ptr = nullptr;

private:
	pointer_type _ptr = absent_ptr;

public:
	constexpr optref() noexcept = default;
	constexpr explicit optref(reference_type ref) noexcept : _ptr(std::addressof(ref)) {}
	constexpr explicit optref(pointer_type ptr) noexcept : _ptr(ptr) {}

	constexpr optref(const optref&) noexcept = default;
	constexpr optref(optref&&) noexcept = default;
	constexpr ~optref() noexcept = default;

	constexpr optref& operator= (const optref&) noexcept = default;
	constexpr optref& operator= (optref&&) noexcept = default;

	constexpr bool operator== (const optref&) const noexcept = default;


	constexpr operator bool() const { return _ptr != absent_ptr; }
	constexpr bool operator! () const { return _ptr == absent_ptr; }

	constexpr reference_type operator* () const { return *_ptr; }

	constexpr pointer_type operator-> () const { return _ptr; }


	constexpr bool present() const { return _ptr != absent_ptr; }

	constexpr reference_type get() const { return *_ptr; }


	static constexpr optref empty() { return {}; }

	static constexpr optref of(reference_type ref) { return optref(ref); }
	static constexpr optref of(pointer_type ptr) { return optref(ptr); }
};


template <typename _Ty>
using const_optref = optref<const _Ty>;
