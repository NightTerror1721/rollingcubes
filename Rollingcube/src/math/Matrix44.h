#pragma once

#include <concepts>
#include <cstdint>
#include <ranges>

#include "MathUtils.h"
#include "Vector4.h"
#include "Vector3.h"


template <std::floating_point _Ty>
class matrix44;

using matrix44f = matrix44<float>;


template <std::floating_point _Ty>
class matrix44
{
public:
	using value_type = _Ty;

	static constexpr std::size_t rows_count = 4;
	static constexpr std::size_t columns_count = 4;
	static constexpr std::size_t vectors_count = 4;
	static constexpr std::size_t values_count = rows_count * columns_count;

	union
	{
		struct
		{
			value_type p00, p01, p02, p03;
			value_type p10, p11, p12, p13;
			value_type p20, p21, p22, p23;
			value_type p30, p31, p32, p33;
		};
		struct
		{
			vector4<value_type> v0;
			vector4<value_type> v1;
			vector4<value_type> v2;
			vector4<value_type> v3;
		};
		vector4<value_type> vectors[vectors_count];
		value_type m[rows_count][columns_count];
		value_type array[values_count];
	};


	constexpr matrix44() noexcept : array{
			value_type(1), value_type(0), value_type(0), value_type(0),
			value_type(0), value_type(1), value_type(0), value_type(0),
			value_type(0), value_type(0), value_type(1), value_type(0),
			value_type(0), value_type(0), value_type(0), value_type(1)
	} {}
	constexpr matrix44(const matrix44&) noexcept = default;
	constexpr matrix44(matrix44&&) noexcept = default;
	constexpr ~matrix44() noexcept = default;

	constexpr matrix44& operator= (const matrix44&) noexcept = default;
	constexpr matrix44& operator= (matrix44&&) noexcept = default;

	constexpr bool operator== (const matrix44&) const noexcept = default;
	constexpr auto operator<=> (const matrix44&) const noexcept = default;


	template <utils::numerical _Ty>
	constexpr matrix44(std::initializer_list<_Ty> values) : matrix44()
	{
		size_t idx = 0;
		for (_Ty value : values)
		{
			array[idx] = value_type(value);
			++idx;
		}
	}

	template <utils::numerical _Ty>
	constexpr matrix44(const _Ty* values, std::size_t count) : matrix44()
	{
		for (std::size_t i = 0; i < count; ++i)
			array[i] = i;
	}

	template <utils::numerical _Ty>
	constexpr matrix44(const _Ty* values) : matrix44(values, 16) {}

	template <typename _Ty> requires
		std::ranges::input_range<_Ty> &&
		utils::numerical<std::ranges::range_value_t<_Ty>>
	constexpr matrix44(const _Ty& range) : matrix44()
	{
		size_t idx = 0;
		auto it = std::ranges::begin(range);
		const auto sentinel = std::ranges::end(range);
		for (; it != sentinel; ++it)
		{
			array[idx] = value_type(*it);
			++idx;
		}
	}



	template <utils::numerical _Ty2>
	friend constexpr matrix44 operator* (const matrix44& left, const matrix44<_Ty2>& right)
	{
		matrix44 res;

		for(unsigned int i = 0; i < 4; ++i)
			for (unsigned int j = 0; j < 4; ++j)
			{
				res.m[i][j] = value_type(0);
				for (unsigned int k = 0; k < 4; ++k)
					res.m[i][j] += left.m[i][k] * value_type(right.m[k][j]);
			}

		return res;
	}



	template <utils::numerical _Ty0>
	friend constexpr vector3<value_type> operator* (const matrix44& left, const vector3<_Ty0>& right)
	{
		return {
			left.p00 * value_type(right.x) + left.p10 * value_type(right.y) + left.p20 * value_type(right.z) + left.p30,
			left.p01 * value_type(right.x) + left.p11 * value_type(right.y) + left.p21 * value_type(right.z) + left.p31,
			left.p02 * value_type(right.x) + left.p12 * value_type(right.y) + left.p22 * value_type(right.z) + left.p32
		};
	}



	constexpr vector3<value_type> right_vector() const { return vector3<value_type>(v0); }
	constexpr vector3<value_type> top_vector() const { return vector3<value_type>(v1); }
	constexpr vector3<value_type> front_vector() const { return vector3<value_type>(v2); }



	constexpr matrix44& clear()
	{
		p00 = value_type(0); p01 = value_type(0); p02 = value_type(0); p03 = value_type(0);
		p10 = value_type(0); p11 = value_type(0); p12 = value_type(0); p13 = value_type(0);
		p20 = value_type(0); p21 = value_type(0); p22 = value_type(0); p23 = value_type(0);
		p30 = value_type(0); p31 = value_type(0); p32 = value_type(0); p33 = value_type(0);
		return *this;
	}

	constexpr matrix44& set_identity()
	{
		p00 = value_type(1); p01 = value_type(0); p02 = value_type(0); p03 = value_type(0);
		p10 = value_type(0); p11 = value_type(1); p12 = value_type(0); p13 = value_type(0);
		p20 = value_type(0); p21 = value_type(0); p22 = value_type(1); p23 = value_type(0);
		p30 = value_type(0); p31 = value_type(0); p32 = value_type(0); p33 = value_type(1);
		return *this;
	}

	constexpr matrix44& transpose()
	{
		std::swap(p01, p10); std::swap(p02, p20); std::swap(p03, p30);
		std::swap(p12, p21); std::swap(p13, p31);
		std::swap(p23, p32);
		return *this;
	}


	template <utils::numerical _Ty0, utils::numerical _Ty1, utils::numerical _Ty2>
	constexpr void set_translation(_Ty0 x, _Ty1 y, _Ty2 z)
	{
		set_identity();
		p30 = value_type(x);
		p31 = value_type(y);
		p32 = value_type(z);
	}

	template <utils::numerical _Ty0, utils::numerical _Ty1, utils::numerical _Ty2>
	static constexpr matrix44 translation(_Ty0 x, _Ty1 y, _Ty2 z)
	{
		matrix44 mat;
		mat.set_translation(x, y, z);
		return mat;
	}

	template <utils::numerical _Ty0, utils::numerical _Ty1, utils::numerical _Ty2>
	constexpr matrix44& translate(_Ty0 x, _Ty1 y, _Ty2 z)
	{
		return (*this = *this * translation(x, y, z));
	}

	template <utils::numerical _Ty0, utils::numerical _Ty1, utils::numerical _Ty2>
	constexpr matrix44& translate_local(_Ty0 x, _Ty1 y, _Ty2 z)
	{
		return (*this = translation(x, y, z) * *this);
	}


	template <utils::numerical _Ty0, utils::numerical _Ty1, utils::numerical _Ty2>
	constexpr void set_scalation(_Ty0 x, _Ty1 y, _Ty2 z)
	{
		set_identity();
		p00 = value_type(x);
		p11 = value_type(y);
		p22 = value_type(z);
	}

	template <utils::numerical _Ty0, utils::numerical _Ty1, utils::numerical _Ty2>
	static constexpr matrix44 scalation(_Ty0 x, _Ty1 y, _Ty2 z)
	{
		matrix44 mat;
		mat.set_scalation(x, y, z);
		return mat;
	}

	template <utils::numerical _Ty0, utils::numerical _Ty1, utils::numerical _Ty2>
	constexpr matrix44& scale(_Ty0 x, _Ty1 y, _Ty2 z)
	{
		return (*this = *this * scalation(x, y, z));
	}

	template <utils::numerical _Ty0, utils::numerical _Ty1, utils::numerical _Ty2>
	constexpr matrix44& scale_local(_Ty0 x, _Ty1 y, _Ty2 z)
	{
		return (*this = scalation(x, y, z) * *this);
	}


	template <utils::numerical _Ty0>
	constexpr void set_rotation(radians_t rad_angle, const vector3<_Ty0>& axis)
	{
		clear();
		vector3<value_type> axis_n = vector3<value_type>(axis).normalize();

		double c = std::cos(rad_angle);
		double s = std::sin(rad_angle);
		double t = 1 - c;

		p00 = value_type(t * axis_n.x * axis_n.x + c);
		p01 = value_type(t * axis_n.x * axis_n.y - s * axis_n.z);
		p02 = value_type(t * axis_n.x * axis_n.z + s * axis_n.y);

		p10 = value_type(t * axis_n.x * axis_n.y + s * axis_n.z);
		p11 = value_type(t * axis_n.y * axis_n.y + c);
		p12 = value_type(t * axis_n.y * axis_n.z - s * axis_n.x);

		p20 = value_type(t * axis_n.x * axis_n.z - s * axis_n.y);
		p21 = value_type(t * axis_n.y * axis_n.z + s * axis_n.x);
		p22 = value_type(t * axis_n.z * axis_n.z + c);

		p33 = value_type(1);
	}

	template <utils::numerical _Ty0>
	static constexpr matrix44 rotation(radians_t rad_angle, const vector3<_Ty0>& axis)
	{
		matrix44 mat;
		mat.set_rotation(rad_angle, axis);
		return mat;
	}

	template <utils::numerical _Ty0>
	constexpr matrix44& rotate(radians_t rad_angle, const vector3<_Ty0>& axis)
	{
		return (*this = *this * rotation(rad_angle, axis));
	}

	template <utils::numerical _Ty0>
	constexpr matrix44& rotate_local(radians_t rad_angle, const vector3<_Ty0>& axis)
	{
		return (*this = rotation(rad_angle, axis) * *this);
	}



	template <utils::numerical _Ty0>
	constexpr vector3<value_type> rotate_vector(const vector3<_Ty0>& v)
	{
		matrix44 temp = *this;
		temp.p30 = value_type(0);
		temp.p31 = value_type(0);
		temp.p32 = value_type(0);
		return temp * v;
	}



	constexpr bool inverse()
	{
		matrix44 temp, final;
		final.set_identity();

		temp = (*this);

		unsigned int m, n;
		m = n = 4;

		for (unsigned int i = 0; i < m; i++)
		{
			// Look for largest element in column
			unsigned int swap = i;
			for (unsigned int j = i + 1; j < m; j++)// m or n
			{
				if (value_type(std::abs(temp.m[j][i])) > value_type(std::abs(temp.m[swap][i])))
					swap = j;
			}

			if (swap != i)
			{
				// Swap rows.
				for (unsigned int k = 0; k < n; k++)
				{
					std::swap(temp.m[i][k], temp.m[swap][k]);
					std::swap(final.m[i][k], final.m[swap][k]);
				}
			}

			#define MATRIX_SINGULAR_THRESHOLD 0.00001 //change this if you experience problems with matrices
			if (std::abs(temp.m[i][i]) <= MATRIX_SINGULAR_THRESHOLD)
			{
				final.set_identity();
				return false;
			}
			#undef MATRIX_SINGULAR_THRESHOLD

			double t = 1.0 / double(temp.m[i][i]);

			for (unsigned int k = 0; k < n; k++)//m or n
			{
				temp.m[i][k] *= value_type(t);
				final.m[i][k] *= value_type(t);
			}

			for (unsigned int j = 0; j < m; j++) // m or n
			{
				if (j != i)
				{
					t = double(temp.m[j][i]);
					for (unsigned int k = 0; k < n; k++)//m or n
					{
						temp.m[j][k] -= value_type(double(temp.m[i][k]) * t);
						final.m[j][k] -= value_type(double(final.m[i][k]) * t);
					}
				}
			}
		}

		*this = final;

		return true;
	}



	constexpr vector3<value_type> get_translation() const noexcept { return { p30, p31, p32 }; }

	constexpr matrix44 get_rotation_only() const noexcept
	{
		matrix44 inv = *this;
		inv.inverse();

		return transpose() * inv;
	}



	template <utils::numerical _Ty0, utils::numerical _Ty1, utils::numerical _Ty2>
	constexpr void look_at(const vector3<_Ty0>& eye, const vector3<_Ty1>& center, const vector3<_Ty2>& up)
	{
		auto front = (center - eye).normalize();
		auto right = front.cross(up).normalize();
		auto top = right.cross(front).normalize();

		set_identity();

		p00 = value_type(right.x); p01 = value_type(top.x); p02 = value_type(-front.x);
		p10 = value_type(right.y); p11 = value_type(top.y); p12 = value_type(-front.y);
		p20 = value_type(right.z); p21 = value_type(top.z); p22 = value_type(-front.z);

		translate_local(-eye.x, -eye.y, -eye.z);
	}



	template <utils::numerical _Ty0, utils::numerical _Ty1, utils::numerical _Ty2, utils::numerical _Ty3>
	constexpr void perspective(_Ty0 fov, _Ty1 aspect, _Ty2 near_plane, _Ty3 far_plane)
	{
		set_identity();

		double f = 1.0 / std::tan(utils::radians(double(fov)) * 0.5);

		p00 = value_type(f / double(aspect));
		p11 = value_type(f);
		p22 = value_type((double(far_plane) * double(near_plane)) / (double(near_plane) - double(far_plane)));
		p23 = value_type(-1);
		p32 = value_type(2.0 * (double(far_plane) * double(near_plane)) / (double(near_plane) - double(far_plane)));
		p33 = value_type(0);
	}



	template <
		utils::numerical _Ty0,
		utils::numerical _Ty1,
		utils::numerical _Ty2,
		utils::numerical _Ty3,
		utils::numerical _Ty4,
		utils::numerical _Ty5
	>
	constexpr void ortho(_Ty0 left, _Ty1 right, _Ty2 bottom, _Ty3 top, _Ty4 near_plane, _Ty5 far_plane)
	{
		clear();

		p00 = value_type(2.0 / (double(right) - double(left)));
		p03 = value_type(-(double(right) + double(left)) / double(right) - double(left));
		p11 = value_type(2.0 / (double(top) - double(bottom)));
		p13 = value_type(-(double(top) + double(bottom)) / double(top) - double(bottom));
		p22 = value_type(2.0 / (double(far_plane) - double(near_plane)));
		p23 = value_type(-(double(far_plane) + double(near_plane)) / double(far_plane) - double(near_plane));
		p33 = value_type(1);
	}


	template <utils::numerical _Ty0>
	constexpr vector3<value_type> project(const vector3<_Ty0>& v) const
	{
		double x = double(p00 * value_type(v.x) + p10 * value_type(v.y) + p20 * value_type(v.z) + p30);
		double y = double(p01 * value_type(v.x) + p11 * value_type(v.y) + p21 * value_type(v.z) + p31);
		double z = double(p02 * value_type(v.x) + p12 * value_type(v.y) + p22 * value_type(v.z) + p32);
		double w = double(p03 * value_type(v.x) + p13 * value_type(v.y) + p23 * value_type(v.z) + p33);

		return {
			value_type((x / w + 1.0) / 2.0),
			value_type((y / w + 1.0) / 2.0),
			value_type((z / w + 1.0) / 2.0)
		};
	}


	constexpr bool get_xyz(value_type* euler) const
	{
		// Code adapted from www.geometrictools.com
		//	Matrix3<Real>::EulerResult Matrix3<Real>::ToEulerAnglesXYZ 
		// +-           -+   +-                                        -+
		// | r00 r01 r02 |   |  cy*cz           -cy*sz            sy    |
		// | r10 r11 r12 | = |  cz*sx*sy+cx*sz   cx*cz-sx*sy*sz  -cy*sx |
		// | r20 r21 r22 |   | -cx*cz*sy+sx*sz   cz*sx+cx*sy*sz   cx*cy |
		// +-           -+   +-                                        -+
		if (p02 < 1.0f)
		{
			if (p02 > -1.0f)
			{
				// y_angle = asin(r02)
				// x_angle = atan2(-r12,r22)
				// z_angle = atan2(-r01,r00)
				euler[1] = value_type(std::asin(p02));
				euler[0] = value_type(std::atan2(-p12, p22));
				euler[2] = value_type(std::atan2(-p01, p00));
				return true;
			}
			else
			{
				// y_angle = -pi/2
				// z_angle - x_angle = atan2(r10,r11)
				// WARNING.  The solution is not unique.  Choosing z_angle = 0.
				euler[1] = value_type(-utils::mid_pi);
				euler[0] = value_type(-std::atan2(p10, p11));
				euler[2] = value_type(0);
				return false;
			}
		}
		else
		{
			// y_angle = +pi/2
			// z_angle + x_angle = atan2(r10,r11)
			// WARNING.  The solutions is not unique.  Choosing z_angle = 0.
			euler[1] = value_type(utils::mid_pi);
			euler[0] = value_type(-std::atan2(p10, p11));
			euler[2] = value_type(0);
		}
		return false;
	}



	template <utils::numerical _Ty0>
	constexpr void set_up_and_ortonormalize(vector3<_Ty0> up)
	{
		up = up.normalize();

		//put the up vector in the matrix
		p10 = value_type(up.x);
		p11 = value_type(up.y);
		p12 = value_type(up.z);

		//orthonormalize
		vector3<value_type> right, front;
		right = right_vector();

		if (std::abs(right.dot(up)) < 0.99998)
		{
			right = vector3<value_type>(up.cross(front_vector()));
			front = right.cross(up);
		}
		else
		{
			front = vector3<value_type>(right).cross(up);
			right = vector3<value_type>(up.cross(front));
		}

		right = right.normalize();
		front = front.normalize();

		p20 = front.x;
		p21 = front.y;
		p22 = front.z;

		p00 = right.x;
		p01 = right.y;
		p02 = right.z;
	}


	template <utils::numerical _Ty0>
	constexpr void set_front_and_ortonormalize(vector3<_Ty0> front)
	{
		front = front.normalize();

		//put the up vector in the matrix
		p20 = value_type(front.x);
		p21 = value_type(front.y);
		p22 = value_type(front.z);

		//orthonormalize
		vector3<value_type> right, up;
		right = right_vector();

		if (std::abs(right.dot(front)) < 0.99998)
		{
			right = top_vector().cross(front);
			up = vector3<value_type>(front.cross(right));
		}
		else
		{
			up = vector3<value_type>(front.cross(right));
			right = up.cross(front);
		}

		right = right.normalize();
		up = up.normalize();

		p10 = up.x;
		p11 = up.y;
		p12 = up.z;

		p00 = right.x;
		p01 = right.y;
		p02 = right.z;
	}

	inline void gl_mult() const
	{
		if constexpr (std::same_as<float, value_type>)
			glMultMatrixf(array);
		else if constexpr (std::same_as<double, value_type>)
			glMultMatrixd(array);
		else
			static_assert(!utils::numerical<value_type>, "INVALID MATRIX TYPE TO MULT IN GL");
	}

	inline void gl_load() const
	{
		if constexpr (std::same_as<float, value_type>)
			glLoadMatrixf(array);
		else if constexpr (std::same_as<double, value_type>)
			glLoadMatrixd(array);
		else
			static_assert(!utils::numerical<value_type>, "INVALID MATRIX TYPE TO LOAD IN GL");
	}



	constexpr value_type* operator& () noexcept { return array; }
	constexpr const value_type* operator& () const noexcept { return array; }
};
