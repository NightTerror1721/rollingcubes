#pragma once

#include <compare>
#include <cstdint>

#include "MathUtils.h"


template <utils::numerical _Ty>
class vector2;


using vector2i = vector2<std::int32_t>;
using vector2u = vector2<std::uint32_t>;
using vector2f = vector2<float>;


template <utils::numerical _Ty>
class vector2
{
public:
	using value_type = _Ty;
	static constexpr std::size_t dimension_count = 2;

	union
	{
		struct
		{
			value_type x;
			value_type y;
		};
		value_type array[2] = { value_type(0), value_type(0)  };
	};



	constexpr vector2() noexcept : x(value_type(0)), y(value_type(0)) {}
	constexpr vector2(const vector2&) noexcept = default;
	constexpr vector2(vector2&&) noexcept = default;
	constexpr ~vector2() noexcept = default;

	constexpr vector2& operator= (const vector2&) noexcept = default;
	constexpr vector2& operator= (vector2&&) noexcept = default;

	constexpr bool operator== (const vector2&) const noexcept = default;
	constexpr auto operator<=> (const vector2&) const noexcept = default;


	constexpr vector2(value_type value) noexcept : x(value), y(value) {}
	constexpr vector2(value_type x, value_type y) noexcept : x(x), y(y) {}
	
	template <utils::dim2vec<value_type> _Ty>
	constexpr explicit vector2(const _Ty& dim2_value) noexcept : x(value_type(dim2_value.x)), y(value_type(dim2_value.y)) {}

	template <utils::numerical _Ty>
	constexpr vector2(_Ty value) noexcept : x(value_type(value)), y(value_type(value)) {}

	template <utils::numerical _Ty, utils::numerical _Ty2>
	constexpr vector2(_Ty x, _Ty2 y) noexcept : x(value_type(x)), y(value_type(y)) {}



	template <utils::dim2vec<value_type> _Ty2>
	friend constexpr vector2 operator+ (const vector2& left, const _Ty2& right)
	{
		return { left.x + value_type(right.x), left.y + value_type(right.y) };
	}

	template <utils::numerical _Ty2>
	friend constexpr vector2 operator+ (const vector2& left, _Ty2 right)
	{
		return { left.x + value_type(right), left.y + value_type(right) };
	}

	template <utils::numerical _Ty2>
	friend constexpr vector2 operator+ (_Ty2 left, const vector2& right)
	{
		return { value_type(left) + right.x, value_type(left) + right.y };
	}


	template <utils::dim2vec<value_type> _Ty2>
	friend constexpr vector2 operator- (const vector2& left, const _Ty2& right)
	{
		return { left.x - value_type(right.x), left.y - value_type(right.y) };
	}

	template <utils::numerical _Ty2>
	friend constexpr vector2 operator- (const vector2& left, _Ty2 right)
	{
		return { left.x - value_type(right), left.y - value_type(right) };
	}

	template <utils::numerical _Ty2>
	friend constexpr vector2 operator- (_Ty2 left, const vector2& right)
	{
		return { value_type(left) - right.x, value_type(left) - right.y };
	}


	template <utils::dim2vec<value_type> _Ty2>
	friend constexpr vector2 operator* (const vector2& left, const _Ty2& right)
	{
		return { left.x * value_type(right.x), left.y * value_type(right.y) };
	}

	template <utils::numerical _Ty2>
	friend constexpr vector2 operator* (const vector2& left, _Ty2 right)
	{
		return { left.x * value_type(right), left.y * value_type(right) };
	}

	template <utils::numerical _Ty2>
	friend constexpr vector2 operator* (_Ty2 left, const vector2& right)
	{
		return { value_type(left) * right.x, value_type(left) * right.y };
	}


	template <utils::dim2vec<value_type> _Ty2>
	friend constexpr vector2 operator/ (const vector2& left, const _Ty2& right)
	{
		return { left.x / value_type(right.x), left.y / value_type(right.y) };
	}

	template <utils::numerical _Ty2>
	friend constexpr vector2 operator/ (const vector2& left, _Ty2 right)
	{
		return { left.x / value_type(right), left.y / value_type(right) };
	}

	template <utils::numerical _Ty2>
	friend constexpr vector2 operator/ (_Ty2 left, const vector2& right)
	{
		return { value_type(left) / right.x, value_type(left) / right.y };
	}



	template <utils::dim2vec<value_type> _Ty2>
	friend constexpr vector2& operator+= (vector2& left, const _Ty2& right)
	{
		left.x += value_type(right.x);
		left.y += value_type(right.y);
		return left;
	}

	template <utils::numerical _Ty2>
	friend constexpr vector2& operator+= (vector2& left, _Ty2 right)
	{
		left.x += value_type(right);
		left.y += value_type(right);
		return left;
	}


	template <utils::dim2vec<value_type> _Ty2>
	friend constexpr vector2& operator-= (vector2& left, const _Ty2& right)
	{
		left.x -= value_type(right.x);
		left.y -= value_type(right.y);
		return left;
	}

	template <utils::numerical _Ty2>
	friend constexpr vector2& operator-= (vector2& left, _Ty2 right)
	{
		left.x -= value_type(right);
		left.y -= value_type(right);
		return left;
	}


	template <utils::dim2vec<value_type> _Ty2>
	friend constexpr vector2& operator*= (vector2& left, const _Ty2& right)
	{
		left.x *= value_type(right.x);
		left.y *= value_type(right.y);
		return left;
	}

	template <utils::numerical _Ty2>
	friend constexpr vector2& operator*= (vector2& left, _Ty2 right)
	{
		left.x *= value_type(right);
		left.y *= value_type(right);
		return left;
	}


	template <utils::dim2vec<value_type> _Ty2>
	friend constexpr vector2& operator/= (vector2& left, const _Ty2& right)
	{
		left.x /= value_type(right.x);
		left.y /= value_type(right.y);
		return left;
	}

	template <utils::numerical _Ty2>
	friend constexpr vector2& operator/= (vector2& left, _Ty2 right)
	{
		left.x /= value_type(right);
		left.y /= value_type(right);
		return left;
	}



	constexpr double length() const { return std::sqrt(double(x * x + y * y)); }

	constexpr double sqrt_length() const noexcept { return double(x * x + y * y); }

	constexpr double distance(const vector2& to) const { return (*this - to).length(); }

	constexpr double sqrt_distance(const vector2& to) const noexcept { return (*this - to).sqrt_length(); }

	constexpr vector2 normalize() const
	{
		auto mag = length();
		return { x / mag, y / mag };
	}

	constexpr value_type dot(const vector2& other) const noexcept { return x * other.x + y * other.y; }

	constexpr double angle(const vector2& to) const
	{
		return std::acos(normalize().dot(to.normalize()));
	}



	constexpr value_type* operator& () { return array; }
	constexpr const value_type* operator& () const { return array; }

	constexpr value_type& operator[] (int index) { return array[index]; }
	constexpr const value_type& operator[] (int index) const { return array[index]; }
};
