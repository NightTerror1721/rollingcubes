#pragma once

#include <cstdint>
#include <algorithm>
#include <concepts>

#include "glm.h"


class alignas(4) Color
{
public:
	using int_component_type = std::uint8_t;
	using int_form_type = std::uint32_t;
	using float_component_type = float;

	static constexpr int_component_type imax = 255u;
	static constexpr int_component_type imin = 0u;

	static constexpr float_component_type fmax = 1.f;
	static constexpr float_component_type fmin = 0.f;

private:
	static constexpr int_component_type clamp(int_component_type value) { return value & imax; }
	static constexpr float_component_type clamp(float_component_type value) { return std::min(fmax, std::max(fmin, value)); }

	static constexpr float_component_type i2f(int_component_type value) { return clamp(float_component_type(value) / float_component_type(imax)); }
	static constexpr int_component_type f2i(float_component_type value) { return clamp(int_component_type(value * float_component_type(imax) + .5f)); }

public:
	union
	{
		int_component_type array[4];
		struct
		{
			int_component_type r;
			int_component_type g;
			int_component_type b;
			int_component_type a;
		};
		int_form_type code = 0;
	};

	constexpr Color() noexcept = default;
	constexpr Color(const Color&) noexcept = default;
	constexpr Color(Color&&) noexcept = default;
	constexpr ~Color() noexcept = default;

	constexpr Color& operator= (const Color&) noexcept = default;
	constexpr Color& operator= (Color&&) noexcept = default;

	constexpr bool operator== (const Color&) const noexcept = default;


	constexpr Color(int_component_type red, int_component_type green, int_component_type blue, int_component_type alpha = imax) :
		array{ clamp(red), clamp(green), clamp(blue), clamp(alpha) }
	{}

	constexpr explicit Color(int_form_type code) : code(code) {}

	constexpr Color(int_form_type code, bool has_alpha) : code(has_alpha ? code : (code | 0xff000000u)) {}

	template <std::floating_point _Ty>
	constexpr Color(_Ty red, _Ty green, _Ty blue, _Ty alpha = _Ty(fmax)) :
		array{
				f2i(float_component_type(red)),
				f2i(float_component_type(green)),
				f2i(float_component_type(blue)),
				f2i(float_component_type(alpha))
		}
	{}


	template <std::integral T, glm::qualifier Q>
	constexpr operator glm::vec<4, T, Q>() const { return { T(r) ,T(g), T(b), T(a) }; }

	template <std::floating_point T, glm::qualifier Q>
	constexpr operator glm::vec<4, T, Q>() const { return { T(i2f(r)) ,T(i2f(g)), T(i2f(b)), T(i2f(a)) }; }

	template <std::integral T, glm::qualifier Q>
	constexpr operator glm::vec<3, T, Q>() const { return { T(r) ,T(g), T(b) }; }

	template <std::floating_point T, glm::qualifier Q>
	constexpr operator glm::vec<3, T, Q>() const { return { T(i2f(r)) ,T(i2f(g)), T(i2f(b)) }; }


	
	inline void gl_color(bool use_alpha) const
	{
		if (use_alpha)
			glColor4ubv(array);
		else
			glColor3ubv(array);
	}

	template <bool _UseAlpha = true>
	inline void gl_color() const
	{
		if constexpr (_UseAlpha)
			glColor4ubv(array);
		else
			glColor3ubv(array);
	}

	

	static constexpr Color white() { return { 255, 255, 255 }; }
	static constexpr Color light_gray() { return { 192, 192, 192 }; }
	static constexpr Color gray() { return { 128, 128, 128 }; }
	static constexpr Color dark_gray() { return { 64, 64, 64 }; }
	static constexpr Color black() { return { 0, 0, 0 }; }
	static constexpr Color red() { return { 255, 0, 0 }; }
	static constexpr Color pink() { return { 255, 175, 175 }; }
	static constexpr Color orange() { return { 255, 200, 0 }; }
	static constexpr Color yellow() { return { 255, 255, 0 }; }
	static constexpr Color green() { return { 0, 255, 0 }; }
	static constexpr Color magenta() { return { 255, 0, 255 }; }
	static constexpr Color cyan() { return { 0, 255, 255 }; }
	static constexpr Color blue() { return { 0, 0, 255 }; }
};
