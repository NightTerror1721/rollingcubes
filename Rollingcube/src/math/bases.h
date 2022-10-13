#pragma once

#include "glm.h"


class CoordSystem
{
private:
	glm::vec3 _xAxis = { 1, 0, 0 };
	glm::vec3 _yAxis = { 0, 1, 0 };
	glm::vec3 _zAxis = { 0, 0, 1 };

public:
	constexpr CoordSystem() noexcept = default;
	constexpr CoordSystem(const CoordSystem&) noexcept = default;
	constexpr CoordSystem(CoordSystem&&) noexcept = default;
	constexpr ~CoordSystem() noexcept = default;

	constexpr CoordSystem& operator= (const CoordSystem&) noexcept = default;
	constexpr CoordSystem& operator= (CoordSystem&&) noexcept = default;

public:
	constexpr const glm::vec3& xAxis() const noexcept { return _xAxis; }
	constexpr const glm::vec3& yAxis() const noexcept { return _yAxis; }
	constexpr const glm::vec3& zAxis() const noexcept { return _zAxis; }

	inline CoordSystem rotate(const glm::vec3& angles) const
	{
		return applyRotation(glm::radians(angles));
	}

	inline CoordSystem rotate(float xAxisAngle, float yAxisAngle, float zAxisAngle) const
	{
		return rotate({ xAxisAngle, yAxisAngle, zAxisAngle });
	}

	inline CoordSystem rotate(float angle, const glm::vec3& axis) const
	{
		return applyRotation(glm::angleAxis(angle, axis));
	}

	inline CoordSystem rotateX(float angle) const { return rotate(angle, { 1, 0, 0 }); }
	inline CoordSystem rotateY(float angle) const { return rotate(angle, { 0, 1, 0 }); }
	inline CoordSystem rotateZ(float angle) const { return rotate(angle, { 0, 0, 1 }); }

private:
	constexpr CoordSystem(const glm::vec3& xAxis, const glm::vec3& yAxis, const glm::vec3& zAxis) :
		_xAxis(xAxis), _yAxis(yAxis), _zAxis(zAxis)
	{}

	inline CoordSystem applyRotation(const glm::quat& rot) const
	{
		CoordSystem cs = {
			adjust(rot * _xAxis),
			adjust(rot * _yAxis),
			adjust(rot * _zAxis)
		};
		return cs;
	}

	static constexpr float adjust(float value)
	{
		if (glm::abs(value) < 0.00001f)
			return 0.f;
		if (glm::abs(value) > 0.99999f)
			return value < 0.f ? -1.f : 1.f;
		return value;
	}

	static inline glm::vec3 adjust(const glm::vec3& v)
	{
		return glm::normalize(glm::vec3{
			adjust(v.x),
			adjust(v.y),
			adjust(v.z)
		});
	}

public:
	static constexpr CoordSystem origin() noexcept { return {}; }

	static inline CoordSystem from(const glm::vec3& angles) { return origin().rotate(angles); }
	static inline CoordSystem from(float xAxisAngle, float yAxisAngle, float zAxisAngle)
	{
		return origin().rotate(xAxisAngle, yAxisAngle, zAxisAngle);
	}

	static inline CoordSystem fromXAxis(const glm::vec3& axis)
	{
		return origin().applyRotation(glm::utils::rotationBetweenVectors(origin()._xAxis, axis));
	}
	static inline CoordSystem fromXAxis(float xAxisAngle, float yAxisAngle, float zAxisAngle)
	{
		return fromXAxis({ xAxisAngle, yAxisAngle, zAxisAngle });
	}

	static inline CoordSystem fromYAxis(const glm::vec3& axis)
	{
		return origin().applyRotation(glm::utils::rotationBetweenVectors(origin()._yAxis, axis));
	}
	static inline CoordSystem fromYAxis(float xAxisAngle, float yAxisAngle, float zAxisAngle)
	{
		return fromYAxis({ xAxisAngle, yAxisAngle, zAxisAngle });
	}

	static inline CoordSystem fromZAxis(const glm::vec3& axis)
	{
		return origin().applyRotation(glm::utils::rotationBetweenVectors(origin()._zAxis, axis));
	}
	static inline CoordSystem fromZAxis(float xAxisAngle, float yAxisAngle, float zAxisAngle)
	{
		return fromZAxis({ xAxisAngle, yAxisAngle, zAxisAngle });
	}
};
