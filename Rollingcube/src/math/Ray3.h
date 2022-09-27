#pragma once

#include <concepts>
#include <stdint.h>

#include "Vector3.h"


template <utils::numerical _Ty>
class ray3;


using ray3i = ray3<std::int32_t>;
using ray3u = ray3<std::uint32_t>;
using ray3f = ray3<double>;


template <utils::numerical _Ty>
class ray3
{
public:
	using value_type = _Ty;
	using vector3_type = vector3<value_type>;

	vector3_type position;
	vector3_type direction;


	constexpr ray3() noexcept = default;
	constexpr ray3(const ray3&) noexcept = default;
	constexpr ray3(ray3&&) noexcept = default;
	constexpr ~ray3() noexcept = default;

	constexpr ray3& operator= (const ray3&) noexcept = default;
	constexpr ray3& operator= (ray3&&) noexcept = default;

	constexpr bool operator== (const ray3&) const noexcept = default;
	constexpr auto operator<=> (const ray3&) const noexcept = default;


	template <utils::numerical _Ty0, utils::numerical _Ty1>
	constexpr ray3(const vector3<_Ty0>& position, const vector3<_Ty1>& direction) :
		position(vector3_type(position)), direction(vector3_type(direction)) {}


	constexpr vector3<_Ty> plane_collision(const vector3_type& plane_pos, const vector3_type& plane_normal)
	{
		double d = plane_normal.dot(position) + plane_pos.dot(plane_normal);
		d = -(d / plane_normal.dot(direction));
		return position + direction * d;
	}
};
