#pragma once

#include <compare>
#include <cstdint>

#include "MathUtils.h"


template <utils::numerical _Ty>
class vector4;


using vector4i = vector4<std::int32_t>;
using vector4u = vector4<std::uint32_t>;
using vector4f = vector4<float>;


template <utils::numerical _Ty>
class vector4
{
public:
	using value_type = _Ty;
	static constexpr std::size_t dimension_count = 4;

	union
	{
		struct
		{
			value_type x;
			value_type y;
			value_type z;
			value_type w;
		};
		value_type array[4] = { value_type(0), value_type(0), value_type(0), value_type(0) };
	};



	constexpr vector4() noexcept : x(value_type(0)), y(value_type(0)), z(value_type(0)), w(value_type(0)) {}
	constexpr vector4(const vector4&) noexcept = default;
	constexpr vector4(vector4&&) noexcept = default;
	constexpr ~vector4() noexcept = default;

	constexpr vector4& operator= (const vector4&) noexcept = default;
	constexpr vector4& operator= (vector4&&) noexcept = default;

	constexpr bool operator== (const vector4&) const noexcept = default;
	constexpr auto operator<=> (const vector4&) const noexcept = default;


	constexpr vector4(value_type value) noexcept : x(value), y(value), z(value) {}
	constexpr vector4(value_type x, value_type y, value_type z, value_type w) noexcept : x(x), y(y), z(z), w(z) {}

	template <utils::dim4vec<value_type> _Ty>
	constexpr explicit vector4(const _Ty& dim4_value) noexcept :
		x(value_type(dim4_value.x)), y(value_type(dim4_value.y)), z(value_type(dim4_value.z)), w(value_type(dim4_value.w)) {}

	template <utils::numerical _Ty>
	constexpr vector4(_Ty value) noexcept : x(value_type(value)), y(value_type(value)), z(value_type(value)) {}

	template <utils::numerical _Ty, utils::numerical _Ty2, utils::numerical _Ty3, utils::numerical _Ty4>
	constexpr vector4(_Ty x, _Ty2 y, _Ty3 z, _Ty4 w) noexcept : x(value_type(x)), y(value_type(y)), z(value_type(z)), w(value_type(w)) {}


	template <utils::dim2vec<value_type> _Ty>
	constexpr explicit vector4(const _Ty& dim2_value) noexcept : x(value_type(dim2_value.x)), y(value_type(dim2_value.y)), z(value_type(0)), w(value_type(0)) {}

	template <utils::dim3vec<value_type> _Ty>
	constexpr explicit vector4(const _Ty& dim3_value) noexcept :
		x(value_type(dim3_value.x)), y(value_type(dim3_value.y)), z(value_type(dim3_value.z)), w(value_type(0)) {}



	template <utils::dim4vec<value_type> _Ty2>
	friend constexpr vector4 operator+ (const vector4& left, const _Ty2& right)
	{
		return { left.x + value_type(right.x), left.y + value_type(right.y), left.z + value_type(right.z), left.w + value_type(right.w) };
	}

	template <utils::numerical _Ty2>
	friend constexpr vector4 operator+ (const vector4& left, _Ty2 right)
	{
		return { left.x + value_type(right), left.y + value_type(right), left.z + value_type(right), left.w + value_type(right) };
	}

	template <utils::numerical _Ty2>
	friend constexpr vector4 operator+ (_Ty2 left, const vector4& right)
	{
		return { value_type(left) + right.x, value_type(left) + right.y, value_type(left) + right.z, value_type(left) + right.w };
	}


	template <utils::dim4vec<value_type> _Ty2>
	friend constexpr vector4 operator- (const vector4& left, const _Ty2& right)
	{
		return { left.x - value_type(right.x), left.y - value_type(right.y), left.z - value_type(right.z), left.w - value_type(right.w) };
	}

	template <utils::numerical _Ty2>
	friend constexpr vector4 operator- (const vector4& left, _Ty2 right)
	{
		return { left.x - value_type(right), left.y - value_type(right), left.z - value_type(right), left.w - value_type(right) };
	}

	template <utils::numerical _Ty2>
	friend constexpr vector4 operator- (_Ty2 left, const vector4& right)
	{
		return { value_type(left) - right.x, value_type(left) - right.y, value_type(left) - right.z, value_type(left) - right.w };
	}


	template <utils::dim4vec<value_type> _Ty2>
	friend constexpr vector4 operator* (const vector4& left, const _Ty2& right)
	{
		return { left.x * value_type(right.x), left.y * value_type(right.y), left.z * value_type(right.z), left.w * value_type(right.w) };
	}

	template <utils::numerical _Ty2>
	friend constexpr vector4 operator* (const vector4& left, _Ty2 right)
	{
		return { left.x * value_type(right), left.y * value_type(right), left.z * value_type(right), left.w * value_type(right) };
	}

	template <utils::numerical _Ty2>
	friend constexpr vector4 operator* (_Ty2 left, const vector4& right)
	{
		return { value_type(left) * right.x, value_type(left) * right.y, value_type(left) * right.z, value_type(left) * right.w };
	}


	template <utils::dim4vec<value_type> _Ty2>
	friend constexpr vector4 operator/ (const vector4& left, const _Ty2& right)
	{
		return { left.x / value_type(right.x), left.y / value_type(right.y), left.z / value_type(right.z), left.w / value_type(right.w) };
	}

	template <utils::numerical _Ty2>
	friend constexpr vector4 operator/ (const vector4& left, _Ty2 right)
	{
		return { left.x / value_type(right), left.y / value_type(right), left.z / value_type(right), left.w / value_type(right) };
	}

	template <utils::numerical _Ty2>
	friend constexpr vector4 operator/ (_Ty2 left, const vector4& right)
	{
		return { value_type(left) / right.x, value_type(left) / right.y, value_type(left) / right.z, value_type(left) / right.w };
	}



	template <utils::dim4vec<value_type> _Ty2>
	friend constexpr vector4& operator+= (vector4& left, const _Ty2& right)
	{
		left.x += value_type(right.x);
		left.y += value_type(right.y);
		left.z += value_type(right.z);
		left.w += value_type(right.w);
		return left;
	}

	template <utils::numerical _Ty2>
	friend constexpr vector4& operator+= (vector4& left, _Ty2 right)
	{
		left.x += value_type(right);
		left.y += value_type(right);
		left.z += value_type(right);
		left.w += value_type(right);
		return left;
	}


	template <utils::dim4vec<value_type> _Ty2>
	friend constexpr vector4& operator-= (vector4& left, const _Ty2& right)
	{
		left.x -= value_type(right.x);
		left.y -= value_type(right.y);
		left.z -= value_type(right.z);
		left.w -= value_type(right.w);
		return left;
	}

	template <utils::numerical _Ty2>
	friend constexpr vector4& operator-= (vector4& left, _Ty2 right)
	{
		left.x -= value_type(right);
		left.y -= value_type(right);
		left.z -= value_type(right);
		left.w -= value_type(right);
		return left;
	}


	template <utils::dim4vec<value_type> _Ty2>
	friend constexpr vector4& operator*= (vector4& left, const _Ty2& right)
	{
		left.x *= value_type(right.x);
		left.y *= value_type(right.y);
		left.z *= value_type(right.z);
		left.w *= value_type(right.w);
		return left;
	}

	template <utils::numerical _Ty2>
	friend constexpr vector4& operator*= (vector4& left, _Ty2 right)
	{
		left.x *= value_type(right);
		left.y *= value_type(right);
		left.z *= value_type(right);
		left.w *= value_type(right);
		return left;
	}


	template <utils::dim4vec<value_type> _Ty2>
	friend constexpr vector4& operator/= (vector4& left, const _Ty2& right)
	{
		left.x /= value_type(right.x);
		left.y /= value_type(right.y);
		left.z /= value_type(right.z);
		left.w /= value_type(right.w);
		return left;
	}

	template <utils::numerical _Ty2>
	friend constexpr vector4& operator/= (vector4& left, _Ty2 right)
	{
		left.x /= value_type(right);
		left.y /= value_type(right);
		left.z /= value_type(right);
		left.w /= value_type(right);
		return left;
	}



	constexpr double length() const { return std::sqrt(double(x * x + y * y + z * z + w * w)); }

	constexpr double sqrt_length() const noexcept { return double(x * x + y * y + z * z + w * w); }

	constexpr double distance(const vector4& to) const { return (*this - to).length(); }

	constexpr double sqrt_distance(const vector4& to) const noexcept { return (*this - to).sqrt_length(); }

	constexpr vector4 normalize() const
	{
		auto mag = length();
		return { x / mag, y / mag, z / mag, w / mag };
	}

	constexpr value_type dot(const vector4& other) const noexcept { return x * other.x + y * other.y + z * other.z + w * other.w; }

	constexpr double angle(const vector4& to) const
	{
		return std::acos(normalize().dot(to.normalize()));
	}



	constexpr value_type* operator& () { return array; }
	constexpr const value_type* operator& () const { return array; }

	constexpr value_type& operator[] (int index) { return array[index]; }
	constexpr const value_type& operator[] (int index) const { return array[index]; }
};
