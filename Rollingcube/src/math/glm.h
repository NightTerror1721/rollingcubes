#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <vector>

namespace glm::utils
{
	template<typename T, qualifier Q>
	constexpr mat<4, 4, T, Q> translation(const vec<3, T, Q>& delta) { return translate(identity<mat<4, 4, T, Q>>(), delta); }

	template<typename T, qualifier Q>
	constexpr mat<4, 4, T, Q> translation(float x, float y, float z) { return translate(identity<mat<4, 4, T, Q>>(), { x, y, z }); }


	template<typename T, qualifier Q>
	constexpr mat<4, 4, T, Q> scalation(const vec<3, T, Q>& delta) { return scale(identity<mat<4, 4, T, Q>>(), delta); }

	template<typename T, qualifier Q>
	constexpr mat<4, 4, T, Q> scalation(float x, float y, float z) { return scale(identity<mat<4, 4, T, Q>>(), { x, y, z }); }


	template<typename T, qualifier Q>
	constexpr mat<4, 4, T, Q> rotation(float angle, const vec<3, T, Q>& axis) { return rotate(identity<mat<4, 4, T, Q>>(), angle, axis); }

	template<typename T, qualifier Q>
	constexpr mat<4, 4, T, Q> rotation(float angle, float x, float y, float z) { return rotate(identity<mat<4, 4, T, Q>>(), angle, { x, y, z }); }
}

template<typename T, glm::qualifier Q>
constexpr glm::vec<3, T, Q> operator* (const glm::mat<4, 4, T, Q>& left, const glm::vec<3, T, Q>& right)
{
	return glm::vec<3, T, Q>(left * glm::vec<4, T, Q>(right, 1.f));
}

template<glm::length_t L, typename T, glm::qualifier Q>
constexpr T* operator& (glm::vec<L, T, Q>& v) { return &v[0]; }

template<glm::length_t L, typename T, glm::qualifier Q>
constexpr const T* operator& (const glm::vec<L, T, Q>& v) { return &v[0]; }


template<glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
constexpr T* operator& (glm::mat<C, R, T, Q>& v) { return &(v[0]); }

template<glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
constexpr const T* operator& (const glm::mat<C, R, T, Q>& v) { return &(v[0]); }


template<glm::length_t L, typename T, glm::qualifier Q>
constexpr T* operator& (std::vector<glm::vec<L, T, Q>>& v) { return &*v.data(); }

template<glm::length_t L, typename T, glm::qualifier Q>
constexpr const T* operator& (const std::vector<glm::vec<L, T, Q>>& v) { return &*v.data(); }
