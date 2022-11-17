#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <vector>

namespace lua::lib
{
	void registerGlmToLua();
}

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

	inline mat3 normalMatrix(const mat4& model) { return mat3(transpose(inverse(model))); }

	inline quat rotationBetweenVectors(vec3 start, vec3 dest)
	{
		start = normalize(start);
		dest = normalize(dest);

		float cosTheta = dot(start, dest);
		vec3 rotationAxis;

		if (cosTheta < -1 + 0.001f) {
			// special case when vectors in opposite directions :
			// there is no "ideal" rotation axis
			// So guess one; any will do as long as it's perpendicular to start
			// This implementation favors a rotation around the Up axis,
			// since it's often what you want to do.
			rotationAxis = cross(vec3(0.0f, 0.0f, 1.0f), start);
			if (length2(rotationAxis) < 0.01) // bad luck, they were parallel, try again!
				rotationAxis = cross(vec3(1.0f, 0.0f, 0.0f), start);

			rotationAxis = normalize(rotationAxis);
			return angleAxis(glm::radians(180.0f), rotationAxis);
		}

		// Implementation from Stan Melax's Game Programming Gems 1 article
		rotationAxis = cross(start, dest);

		float s = sqrt((1 + cosTheta) * 2);
		float invs = 1 / s;

		return quat(
			s * 0.5f,
			rotationAxis.x * invs,
			rotationAxis.y * invs,
			rotationAxis.z * invs
		);
	}

	inline quat lookAt(vec3 direction, vec3 desiredUp)
	{
		if (length2(direction) < 0.0001f)
			return quat();

		// Recompute desiredUp so that it's perpendicular to the direction
		// You can skip that part if you really want to force desiredUp
		vec3 right = cross(direction, desiredUp);
		desiredUp = cross(right, direction);

		// Find the rotation between the front of the object (that we assume towards +Z,
		// but this depends on your model) and the desired direction
		quat rot1 = rotationBetweenVectors(vec3(0.0f, 0.0f, 1.0f), direction);
		// Because of the 1rst rotation, the up is probably completely screwed up. 
		// Find the rotation between the "up" of the rotated object, and the desired up
		vec3 newUp = rot1 * vec3(0.0f, 1.0f, 0.0f);
		quat rot2 = rotationBetweenVectors(newUp, desiredUp);

		// Apply them
		return rot2 * rot1; // remember, in reverse order.
	}

	inline mat3 pitchYawRollRotationMatrix(float pitch, float yaw, float roll)
	{
		pitch = glm::radians(pitch);
		yaw = glm::radians(yaw);
		roll = glm::radians(roll);

		float cp = cos(pitch), sp = sin(pitch);
		float cy = cos(yaw),   sy = sin(yaw);
		float cr = cos(roll),  sr = sin(roll);

		mat3 m;
		m[0][0] = cp * cy;
		m[1][0] = sp * cy;
		m[2][0] = -sy;
		m[0][1] = cp * sy * sr - sp * cr;
		m[1][1] = sp * sy * sr + cp * cr;
		m[2][1] = cy * sr;
		m[0][2] = cp * sy * cr + sp * sr;
		m[1][2] = sp * sy * sr - cp * sr;
		m[2][2] = cy * cr;

		return m;
	}
	inline mat3 pitchYawRollRotationMatrix(const vec3& eulerAngles)
	{
		return pitchYawRollRotationMatrix(eulerAngles.z, eulerAngles.y, eulerAngles.x);
	}

	template <std::integral _Ty, std::convertible_to<_Ty> _MinTy, std::convertible_to<_Ty> _MaxTy>
	constexpr _Ty normalizeRange(_Ty value, _MinTy min, _MaxTy max)
	{
		return ((value - _Ty(min)) % (_Ty(max) - _Ty(min))) + _Ty(min);
	}

	template <std::floating_point _Ty, std::convertible_to<_Ty> _MinTy, std::convertible_to<_Ty> _MaxTy>
	constexpr _Ty normalizeRange(_Ty value, _MinTy min, _MaxTy max)
	{
		if (value < _Ty(min))
			value += _Ty(std::intmax_t(value / (_Ty(max) - _Ty(min)))) * (_Ty(max) - _Ty(min));
		else if(value > _Ty(max))
			value -= _Ty(std::intmax_t(value / (_Ty(max) - _Ty(min)))) * (_Ty(max) - _Ty(min));
		return value;
	}

	template <typename T, qualifier Q, typename _MinTy, typename _MaxT> requires 
		(std::integral<T> || std::floating_point<T>) &&
		std::convertible_to<_MinTy, T> &&
		std::convertible_to<_MaxT, T>
	constexpr vec<2, T, Q> normalizeRange(const vec<2, T, Q>& value, _MinTy min, _MaxT max)
	{
		return {
			normalizeRange<T, _MinTy, _MaxT>(value.x, min, max),
			normalizeRange<T, _MinTy, _MaxT>(value.y, min, max)
		};
	}

	template <typename T, qualifier Q, typename _MinTy, typename _MaxT> requires
		(std::integral<T> || std::floating_point<T>) &&
		std::convertible_to<_MinTy, T> &&
		std::convertible_to<_MaxT, T>
		constexpr vec<3, T, Q> normalizeRange(const vec<3, T, Q>& value, _MinTy min, _MaxT max)
	{
		return {
			normalizeRange<T, _MinTy, _MaxT>(value.x, min, max),
			normalizeRange<T, _MinTy, _MaxT>(value.y, min, max),
			normalizeRange<T, _MinTy, _MaxT>(value.z, min, max)
		};
	}

	template <typename T, qualifier Q, typename _MinTy, typename _MaxT> requires
		(std::integral<T> || std::floating_point<T>) &&
		std::convertible_to<_MinTy, T> &&
		std::convertible_to<_MaxT, T>
		constexpr vec<4, T, Q> normalizeRange(const vec<4, T, Q>& value, _MinTy min, _MaxT max)
	{
		return {
			normalizeRange<T, _MinTy, _MaxT>(value.x, min, max),
			normalizeRange<T, _MinTy, _MaxT>(value.y, min, max),
			normalizeRange<T, _MinTy, _MaxT>(value.z, min, max),
			normalizeRange<T, _MinTy, _MaxT>(value.w, min, max)
		};
	}

	inline glm::vec3 pitchYawDirection(float pitch, float yaw)
	{
		pitch = glm::utils::normalizeRange(pitch, -180, 180);
		yaw = glm::utils::normalizeRange(yaw, -180, 180);
		return glm::normalize(glm::vec3{
			glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch)),
			glm::sin(glm::radians(pitch)),
			glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch))
		});
	}
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
