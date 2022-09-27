#pragma once

#include <cmath>
#include <GLFW/glfw3.h>

union Frustum
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
};
