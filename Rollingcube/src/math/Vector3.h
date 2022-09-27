#pragma once

#include <compare>
#include <cstdint>

#include "MathUtils.h"


template <utils::numerical _Ty>
class vector3;


using vector3i = vector3<std::int32_t>;
using vector3u = vector3<std::uint32_t>;
using vector3f = vector3<float>;


template <utils::numerical _Ty>
class vector3
{
public:
	using value_type = _Ty;
	static constexpr std::size_t dimension_count = 3;

	union
	{
		struct
		{
			value_type x;
			value_type y;
			value_type z;
		};
		value_type array[3] = { value_type(0), value_type(0), value_type(0) };
	};



	constexpr vector3() noexcept : x(value_type(0)), y(value_type(0)), z(value_type(0)) {}
	constexpr vector3(const vector3&) noexcept = default;
	constexpr vector3(vector3&&) noexcept = default;
	constexpr ~vector3() noexcept = default;

	constexpr vector3& operator= (const vector3&) noexcept = default;
	constexpr vector3& operator= (vector3&&) noexcept = default;

	constexpr bool operator== (const vector3&) const noexcept = default;
	constexpr auto operator<=> (const vector3&) const noexcept = default;


	constexpr vector3(value_type value) noexcept : x(value), y(value), z(value) {}
	constexpr vector3(value_type x, value_type y, value_type z) noexcept : x(x), y(y), z(z) {}

	template <utils::dim3vec<value_type> _Ty>
	constexpr explicit vector3(const _Ty& dim3_value) noexcept : x(value_type(dim3_value.x)), y(value_type(dim3_value.y)), z(value_type(dim3_value.z)) {}

	template <utils::numerical _Ty>
	constexpr vector3(_Ty value) noexcept : x(value_type(value)), y(value_type(value)), z(value_type(value)) {}

	template <utils::numerical _Ty, utils::numerical _Ty2, utils::numerical _Ty3>
	constexpr vector3(_Ty x, _Ty2 y, _Ty3 z) noexcept : x(value_type(x)), y(value_type(y)), z(value_type(z)) {}


	template <utils::dim2vec<value_type> _Ty>
	constexpr explicit vector3(const _Ty& dim2_value) noexcept : x(value_type(dim2_value.x)), y(value_type(dim2_value.y)), z(value_type(0)) {}



	template <utils::dim3vec<value_type> _Ty2>
	friend constexpr vector3 operator+ (const vector3& left, const _Ty2& right)
	{
		return { left.x + value_type(right.x), left.y + value_type(right.y), left.z + value_type(right.z) };
	}

	template <utils::numerical _Ty2>
	friend constexpr vector3 operator+ (const vector3& left, _Ty2 right)
	{
		return { left.x + value_type(right), left.y + value_type(right), left.z + value_type(right) };
	}

	template <utils::numerical _Ty2>
	friend constexpr vector3 operator+ (_Ty2 left, const vector3& right)
	{
		return { value_type(left) + right.x, value_type(left) + right.y, value_type(left) + right.z };
	}


	template <utils::dim3vec<value_type> _Ty2>
	friend constexpr vector3 operator- (const vector3& left, const _Ty2& right)
	{
		return { left.x - value_type(right.x), left.y - value_type(right.y), left.z - value_type(right.z) };
	}

	template <utils::numerical _Ty2>
	friend constexpr vector3 operator- (const vector3& left, _Ty2 right)
	{
		return { left.x - value_type(right), left.y - value_type(right), left.z - value_type(right) };
	}

	template <utils::numerical _Ty2>
	friend constexpr vector3 operator- (_Ty2 left, const vector3& right)
	{
		return { value_type(left) - right.x, value_type(left) - right.y, value_type(left) - right.z };
	}


	template <utils::dim3vec<value_type> _Ty2>
	friend constexpr vector3 operator* (const vector3& left, const _Ty2& right)
	{
		return { left.x * value_type(right.x), left.y * value_type(right.y), left.z * value_type(right.z) };
	}

	template <utils::numerical _Ty2>
	friend constexpr vector3 operator* (const vector3& left, _Ty2 right)
	{
		return { left.x * value_type(right), left.y * value_type(right), left.z * value_type(right) };
	}

	template <utils::numerical _Ty2>
	friend constexpr vector3 operator* (_Ty2 left, const vector3& right)
	{
		return { value_type(left) * right.x, value_type(left) * right.y, value_type(left) * right.z };
	}


	template <utils::dim3vec<value_type> _Ty2>
	friend constexpr vector3 operator/ (const vector3& left, const _Ty2& right)
	{
		return { left.x / value_type(right.x), left.y / value_type(right.y), left.z / value_type(right.z) };
	}

	template <utils::numerical _Ty2>
	friend constexpr vector3 operator/ (const vector3& left, _Ty2 right)
	{
		return { left.x / value_type(right), left.y / value_type(right), left.z / value_type(right) };
	}

	template <utils::numerical _Ty2>
	friend constexpr vector3 operator/ (_Ty2 left, const vector3& right)
	{
		return { value_type(left) / right.x, value_type(left) / right.y, value_type(left) / right.z };
	}



	template <utils::dim3vec<value_type> _Ty2>
	friend constexpr vector3& operator+= (vector3& left, const _Ty2& right)
	{
		left.x += value_type(right.x);
		left.y += value_type(right.y);
		left.z += value_type(right.z);
		return left;
	}

	template <utils::numerical _Ty2>
	friend constexpr vector3& operator+= (vector3& left, _Ty2 right)
	{
		left.x += value_type(right);
		left.y += value_type(right);
		left.z += value_type(right);
		return left;
	}


	template <utils::dim3vec<value_type> _Ty2>
	friend constexpr vector3& operator-= (vector3& left, const _Ty2& right)
	{
		left.x -= value_type(right.x);
		left.y -= value_type(right.y);
		left.z -= value_type(right.z);
		return left;
	}

	template <utils::numerical _Ty2>
	friend constexpr vector3& operator-= (vector3& left, _Ty2 right)
	{
		left.x -= value_type(right);
		left.y -= value_type(right);
		left.z -= value_type(right);
		return left;
	}


	template <utils::dim3vec<value_type> _Ty2>
	friend constexpr vector3& operator*= (vector3& left, const _Ty2& right)
	{
		left.x *= value_type(right.x);
		left.y *= value_type(right.y);
		left.z *= value_type(right.z);
		return left;
	}

	template <utils::numerical _Ty2>
	friend constexpr vector3& operator*= (vector3& left, _Ty2 right)
	{
		left.x *= value_type(right);
		left.y *= value_type(right);
		left.z *= value_type(right);
		return left;
	}


	template <utils::dim3vec<value_type> _Ty2>
	friend constexpr vector3& operator/= (vector3& left, const _Ty2& right)
	{
		left.x /= value_type(right.x);
		left.y /= value_type(right.y);
		left.z /= value_type(right.z);
		return left;
	}

	template <utils::numerical _Ty2>
	friend constexpr vector3& operator/= (vector3& left, _Ty2 right)
	{
		left.x /= value_type(right);
		left.y /= value_type(right);
		left.z /= value_type(right);
		return left;
	}



	constexpr double length() const { return std::sqrt(double(x * x + y * y + z * z)); }

	constexpr double sqrt_length() const noexcept { return double(x * x + y * y + z * z); }

	constexpr double distance(const vector3& to) const { return (*this - to).length(); }

	constexpr double sqrt_distance(const vector3& to) const noexcept { return (*this - to).sqrt_length(); }

	constexpr vector3 normalize() const
	{
		auto mag = length();
		return { x / mag, y / mag, z / mag };
	}

	constexpr value_type dot(const vector3& other) const noexcept { return x * other.x + y * other.y + z * other.z; }

	constexpr double angle(const vector3& to) const
	{
		return std::acos(normalize().dot(to.normalize()));
	}

	constexpr vector3 cross(const vector3& other) noexcept
	{
		return {
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x
		};
	}



	constexpr value_type* operator& () { return array; }
	constexpr const value_type* operator& () const { return array; }

	constexpr value_type& operator[] (int index) { return array[index]; }
	constexpr const value_type& operator[] (int index) const { return array[index]; }
};



namespace utils
{
	template <numerical _Ty>
	constexpr vector3<_Ty> ray_plane_collision(
		const vector3<_Ty>& plane_pos,
		const vector3<_Ty>& plane_normal,
		const vector3<_Ty>& ray_origin,
		const vector3<_Ty>& ray_dir)
	{
		double d = plane_normal.dot(ray_origin) + plane_pos.dot(plane_normal);
		d = -(d / plane_normal.dot(ray_dir));
		return ray_origin + ray_dir * d;
	}
}
