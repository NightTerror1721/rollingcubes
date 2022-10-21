#pragma once

#include <cmath>
#include <GLFW/glfw3.h>

#include "math/glm.h"


struct FrustumPlane
{
	glm::vec3 normal = { 0, 1, 0 };
	float distance = 0;

	constexpr FrustumPlane() = default;
	constexpr FrustumPlane(const FrustumPlane&) = default;
	constexpr FrustumPlane(FrustumPlane&&) noexcept = default;
	constexpr ~FrustumPlane() = default;

	constexpr FrustumPlane& operator= (const FrustumPlane&) = default;
	constexpr FrustumPlane& operator= (FrustumPlane&&) noexcept = default;

	inline FrustumPlane(const glm::vec3& point, const glm::vec3& norm) :
		normal(glm::normalize(norm)),
		distance(glm::dot(normal, point))
	{}

	constexpr float getSignedDistanceToPlane(const glm::vec3& point) const { return glm::dot(normal, point) - distance; }
};


class Frustum
{
public:
	using Plane = FrustumPlane;

private:
	Plane _top;
	Plane _bottom;

	Plane _right;
	Plane _left;

	Plane _far;
	Plane _near;

public:
	Frustum() = default;
	Frustum(const Frustum&) = default;
	Frustum(Frustum&&) noexcept = default;
	~Frustum() = default;

	Frustum& operator= (const Frustum&) = default;
	Frustum& operator= (Frustum&&) noexcept = default;

public:
	constexpr const Plane& getTop() const { return _top; }
	constexpr const Plane& getBottom() const { return _bottom; }
	constexpr const Plane& getRight() const { return _right; }
	constexpr const Plane& getLeft() const { return _left; }
	constexpr const Plane& getFar() const { return _far; }
	constexpr const Plane& getNear() const { return _near; }

	void extract(
		const glm::vec3& position,
		const glm::vec3& front,
		const glm::vec3& right,
		const glm::vec3& up,
		float aspect,
		float fov,
		float near,
		float far
	);
};




/*class Frustum
{
private:
	enum class Plane : int
	{
		Left = 0,
		Right,
		Bottom,
		Top,
		Near,
		Far,

		Count,
		Combinations = Count * (Count - 1) / 2
	};

	template <Plane i, Plane j>
	struct ij2k
	{
		static constexpr int k = int(i) * (9 - int(i)) / 2 + int(j) - 1;
	};

private:
	glm::vec4 _planes[int(Plane::Count)] = {};
	glm::vec3 _points[8] = {};

public:
	constexpr Frustum() = default;
	constexpr Frustum(const Frustum&) = default;
	constexpr Frustum(Frustum&&) noexcept = default;
	constexpr ~Frustum() = default;

	constexpr Frustum& operator= (const Frustum&) = default;
	constexpr Frustum& operator= (Frustum&&) noexcept = default;

public:
	void extract(glm::mat4 projectionView);

	bool isPointVisible(const glm::vec3& position) const;
	bool isSphereVisible(const glm::vec3& position, float radius) const;

public:
	inline void extract(const glm::mat4& view, const glm::mat4& projection) { extract(view * projection); }
	
private:
	template <Plane a, Plane b, Plane c>
	inline glm::vec3 intersection(const glm::vec3* crosses) const
	{
		float d = glm::dot(glm::vec3(_planes[int(a)]), crosses[int(ij2k<b, c>::k)]);
		glm::vec3 res = glm::mat3(crosses[ij2k<b, c>::k], -crosses[ij2k<a, c>::k], crosses[ij2k<a, b>::k]) *
			glm::vec3(_planes[int(a)].w, _planes[int(b)].w, _planes[int(c)].w);
		return res * (1.f / d);
	}
};*/

/*union Frustum
{
	static constexpr std::size_t plane_edges_count = 4;
	static constexpr std::size_t planes_count = 6;


	struct
	{
		float right[plane_edges_count];
		float left[plane_edges_count];
		float bottom[plane_edges_count];
		float top[plane_edges_count];
		float far[plane_edges_count];
		float near[plane_edges_count];
	};
	float planes[planes_count][plane_edges_count];
	float array[planes_count * plane_edges_count];


	void fillFromGL();

	bool containsPoint(float x, float y, float z) const;
	bool containsSphere(float x, float y, float z, float radius) const;


	inline static Frustum extractFromGL()
	{
		Frustum frustum;
		frustum.fillFromGL();
		return frustum;
	}
};*/
