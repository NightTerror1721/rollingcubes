#include "frustum.h"

void Frustum::extract(
	const glm::vec3& position,
	const glm::vec3& front,
	const glm::vec3& right,
	const glm::vec3& up,
	float aspect,
	float fov,
	float near,
	float far
) {
	const float halfVSide = far * glm::tan(fov * .5f);
	const float halfHSide = halfVSide * aspect;
	const glm::vec3 frontMultFar = far * front;

	_near = { position + near * front, front };
	_far = { position + frontMultFar, -front };
	_right = { position, glm::cross(up, frontMultFar + right * halfHSide) };
	_left = { position, glm::cross(frontMultFar - right * halfHSide, up) };
	_top = { position, glm::cross(right, frontMultFar - up * halfVSide) };
	_bottom = { position, glm::cross(frontMultFar + up * halfVSide, right) };
}







/*void Frustum::extract(glm::mat4 projectionView)
{
	projectionView = glm::transpose(projectionView);

	_planes[int(Plane::Left)]   = projectionView[3] + projectionView[0];
	_planes[int(Plane::Right)]  = projectionView[3] + projectionView[0];
	_planes[int(Plane::Bottom)] = projectionView[3] + projectionView[1];
	_planes[int(Plane::Top)]    = projectionView[3] + projectionView[1];
	_planes[int(Plane::Near)]   = projectionView[3] + projectionView[2];
	_planes[int(Plane::Far)]    = projectionView[3] + projectionView[2];

	glm::vec3 crosses[int(Plane::Combinations)] = {
		glm::cross(glm::vec3(_planes[int(Plane::Left)]),   glm::vec3(_planes[int(Plane::Right)])),
		glm::cross(glm::vec3(_planes[int(Plane::Left)]),   glm::vec3(_planes[int(Plane::Bottom)])),
		glm::cross(glm::vec3(_planes[int(Plane::Left)]),   glm::vec3(_planes[int(Plane::Top)])),
		glm::cross(glm::vec3(_planes[int(Plane::Left)]),   glm::vec3(_planes[int(Plane::Near)])),
		glm::cross(glm::vec3(_planes[int(Plane::Left)]),   glm::vec3(_planes[int(Plane::Far)])),
		glm::cross(glm::vec3(_planes[int(Plane::Right)]),  glm::vec3(_planes[int(Plane::Bottom)])),
		glm::cross(glm::vec3(_planes[int(Plane::Right)]),  glm::vec3(_planes[int(Plane::Top)])),
		glm::cross(glm::vec3(_planes[int(Plane::Right)]),  glm::vec3(_planes[int(Plane::Near)])),
		glm::cross(glm::vec3(_planes[int(Plane::Right)]),  glm::vec3(_planes[int(Plane::Far)])),
		glm::cross(glm::vec3(_planes[int(Plane::Bottom)]), glm::vec3(_planes[int(Plane::Top)])),
		glm::cross(glm::vec3(_planes[int(Plane::Bottom)]), glm::vec3(_planes[int(Plane::Near)])),
		glm::cross(glm::vec3(_planes[int(Plane::Bottom)]), glm::vec3(_planes[int(Plane::Far)])),
		glm::cross(glm::vec3(_planes[int(Plane::Top)]),    glm::vec3(_planes[int(Plane::Near)])),
		glm::cross(glm::vec3(_planes[int(Plane::Top)]),    glm::vec3(_planes[int(Plane::Far)])),
		glm::cross(glm::vec3(_planes[int(Plane::Near)]),   glm::vec3(_planes[int(Plane::Far)]))
	};

	_points[0] = intersection<Plane::Left, Plane::Bottom, Plane::Near>(crosses);
	_points[1] = intersection<Plane::Left, Plane::Top, Plane::Near>(crosses);
	_points[2] = intersection<Plane::Right, Plane::Bottom, Plane::Near>(crosses);
	_points[3] = intersection<Plane::Right, Plane::Top, Plane::Near>(crosses);
	_points[4] = intersection<Plane::Left, Plane::Bottom, Plane::Far>(crosses);
	_points[5] = intersection<Plane::Left, Plane::Top, Plane::Far>(crosses);
	_points[6] = intersection<Plane::Right, Plane::Bottom, Plane::Far>(crosses);
	_points[7] = intersection<Plane::Right, Plane::Top, Plane::Far>(crosses);
}

bool Frustum::isPointVisible(const glm::vec3& position) const
{
#define POINT_OUT_PLANE(p, position) \
(_planes[int(p)].x * position.x + _planes[int(p)].y * position.y + _planes[int(p)].z * position.z + _planes[int(p)].w) <= 0

	if (POINT_OUT_PLANE(Plane::Left, position)) return false;
	if (POINT_OUT_PLANE(Plane::Right, position)) return false;
	if (POINT_OUT_PLANE(Plane::Bottom, position)) return false;
	if (POINT_OUT_PLANE(Plane::Top, position)) return false;
	if (POINT_OUT_PLANE(Plane::Near, position)) return false;
	if (POINT_OUT_PLANE(Plane::Far, position)) return false;

	return true;

#undef POINT_OUT_PLANE
}

bool Frustum::isSphereVisible(const glm::vec3& position, float radius) const
{
#define SPHERE_OUT_PLANE(p, position, radius) \
(_planes[int(p)].x * position.x + _planes[int(p)].y * position.y + _planes[int(p)].z * position.z + _planes[int(p)].w) <= -radius

	if (radius <= 0)
		return true;

	if (SPHERE_OUT_PLANE(Plane::Left, position, radius)) return false;
	if (SPHERE_OUT_PLANE(Plane::Right, position, radius)) return false;
	if (SPHERE_OUT_PLANE(Plane::Bottom, position, radius)) return false;
	if (SPHERE_OUT_PLANE(Plane::Top, position, radius)) return false;
	if (SPHERE_OUT_PLANE(Plane::Near, position, radius)) return false;
	if (SPHERE_OUT_PLANE(Plane::Far, position, radius)) return false;

	return true;

#undef SPHERE_OUT_PLANE
}*/


/*void Frustum::fillFromGL()
{
	float proj[16];
	float modl[16];
	float clip[16];
	float t;

	/* Get the current PROJECTION matrix from OpenGL 
	glGetFloatv(GL_PROJECTION_MATRIX, proj);

	/* Get the current MODELVIEW matrix from OpenGL 
	glGetFloatv(GL_MODELVIEW_MATRIX, modl);

	/* Combine the two matrices (multiply projection by modelview) 
	clip[0] = modl[0] * proj[0] + modl[1] * proj[4] + modl[2] * proj[8] + modl[3] * proj[12];
	clip[1] = modl[0] * proj[1] + modl[1] * proj[5] + modl[2] * proj[9] + modl[3] * proj[13];
	clip[2] = modl[0] * proj[2] + modl[1] * proj[6] + modl[2] * proj[10] + modl[3] * proj[14];
	clip[3] = modl[0] * proj[3] + modl[1] * proj[7] + modl[2] * proj[11] + modl[3] * proj[15];

	clip[4] = modl[4] * proj[0] + modl[5] * proj[4] + modl[6] * proj[8] + modl[7] * proj[12];
	clip[5] = modl[4] * proj[1] + modl[5] * proj[5] + modl[6] * proj[9] + modl[7] * proj[13];
	clip[6] = modl[4] * proj[2] + modl[5] * proj[6] + modl[6] * proj[10] + modl[7] * proj[14];
	clip[7] = modl[4] * proj[3] + modl[5] * proj[7] + modl[6] * proj[11] + modl[7] * proj[15];

	clip[8] = modl[8] * proj[0] + modl[9] * proj[4] + modl[10] * proj[8] + modl[11] * proj[12];
	clip[9] = modl[8] * proj[1] + modl[9] * proj[5] + modl[10] * proj[9] + modl[11] * proj[13];
	clip[10] = modl[8] * proj[2] + modl[9] * proj[6] + modl[10] * proj[10] + modl[11] * proj[14];
	clip[11] = modl[8] * proj[3] + modl[9] * proj[7] + modl[10] * proj[11] + modl[11] * proj[15];

	clip[12] = modl[12] * proj[0] + modl[13] * proj[4] + modl[14] * proj[8] + modl[15] * proj[12];
	clip[13] = modl[12] * proj[1] + modl[13] * proj[5] + modl[14] * proj[9] + modl[15] * proj[13];
	clip[14] = modl[12] * proj[2] + modl[13] * proj[6] + modl[14] * proj[10] + modl[15] * proj[14];
	clip[15] = modl[12] * proj[3] + modl[13] * proj[7] + modl[14] * proj[11] + modl[15] * proj[15];

	/* Extract the numbers for the RIGHT plane 
	planes[0][0] = clip[3] - clip[0];
	planes[0][1] = clip[7] - clip[4];
	planes[0][2] = clip[11] - clip[8];
	planes[0][3] = clip[15] - clip[12];

	/* Normalize the result 
	t = sqrt(planes[0][0] * planes[0][0] + planes[0][1] * planes[0][1] + planes[0][2] * planes[0][2]);
	planes[0][0] /= t;
	planes[0][1] /= t;
	planes[0][2] /= t;
	planes[0][3] /= t;

	/* Extract the numbers for the LEFT plane 
	planes[1][0] = clip[3] + clip[0];
	planes[1][1] = clip[7] + clip[4];
	planes[1][2] = clip[11] + clip[8];
	planes[1][3] = clip[15] + clip[12];

	/* Normalize the result *
	t = sqrt(planes[1][0] * planes[1][0] + planes[1][1] * planes[1][1] + planes[1][2] * planes[1][2]);
	planes[1][0] /= t;
	planes[1][1] /= t;
	planes[1][2] /= t;
	planes[1][3] /= t;

	/* Extract the BOTTOM plane 
	planes[2][0] = clip[3] + clip[1];
	planes[2][1] = clip[7] + clip[5];
	planes[2][2] = clip[11] + clip[9];
	planes[2][3] = clip[15] + clip[13];

	/* Normalize the result 
	t = sqrt(planes[2][0] * planes[2][0] + planes[2][1] * planes[2][1] + planes[2][2] * planes[2][2]);
	planes[2][0] /= t;
	planes[2][1] /= t;
	planes[2][2] /= t;
	planes[2][3] /= t;

	/* Extract the TOP plane 
	planes[3][0] = clip[3] - clip[1];
	planes[3][1] = clip[7] - clip[5];
	planes[3][2] = clip[11] - clip[9];
	planes[3][3] = clip[15] - clip[13];

	/* Normalize the result 
	t = sqrt(planes[3][0] * planes[3][0] + planes[3][1] * planes[3][1] + planes[3][2] * planes[3][2]);
	planes[3][0] /= t;
	planes[3][1] /= t;
	planes[3][2] /= t;
	planes[3][3] /= t;

	/* Extract the FAR plane 
	planes[4][0] = clip[3] - clip[2];
	planes[4][1] = clip[7] - clip[6];
	planes[4][2] = clip[11] - clip[10];
	planes[4][3] = clip[15] - clip[14];

	/* Normalize the result 
	t = sqrt(planes[4][0] * planes[4][0] + planes[4][1] * planes[4][1] + planes[4][2] * planes[4][2]);
	planes[4][0] /= t;
	planes[4][1] /= t;
	planes[4][2] /= t;
	planes[4][3] /= t;

	/* Extract the NEAR plane 
	planes[5][0] = clip[3] + clip[2];
	planes[5][1] = clip[7] + clip[6];
	planes[5][2] = clip[11] + clip[10];
	planes[5][3] = clip[15] + clip[14];

	/* Normalize the result 
	t = sqrt(planes[5][0] * planes[5][0] + planes[5][1] * planes[5][1] + planes[5][2] * planes[5][2]);
	planes[5][0] /= t;
	planes[5][1] /= t;
	planes[5][2] /= t;
	planes[5][3] /= t;
}



bool Frustum::containsPoint(float x, float y, float z) const
{
	for (unsigned int p = 0; p < planes_count; ++p)
		if (planes[p][0] * x + planes[p][1] * y + planes[p][2] * z + planes[p][3] <= 0)
			return false;
	return true;
}

bool Frustum::containsSphere(float x, float y, float z, float radius) const
{
	for (unsigned int p = 0; p < planes_count; ++p)
		if (planes[p][0] * x + planes[p][1] * y + planes[p][2] * z + planes[p][3] <= -radius)
			return false;
	return true;
}*/
