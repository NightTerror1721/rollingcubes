#pragma once

#include "math/glm.h"
#include "frustum.h"


enum class CameraType
{
	Perspective,
	Orthographic
};

class Camera;

glm::mat4 operator* (const Camera& cam, const glm::mat4& model);
glm::mat4 operator* (const glm::mat4& model, const Camera& cam);


class Camera
{
public:
	using Type = CameraType;

private:
	Type _type;

	glm::vec3 _eye;
	glm::vec3 _center;
	glm::vec3 _up;

	float _fov;
	float _aspect;
	float _nearPlane;
	float _farPlane;

	float _left, _right, _top, _bottom;

	glm::mat4 _viewMatrix;
	glm::mat4 _projectionMatrix;
	glm::mat4 _viewprojectionMatrix;

public:
	~Camera() noexcept = default;

	Camera(const Camera&) noexcept = default;
	Camera(Camera&&) noexcept = default;

	Camera& operator= (const Camera&) noexcept = default;
	Camera& operator= (Camera&&) noexcept = default;

	bool operator== (const Camera&) const = default;


	Camera();

	void setToGL();

	void move(const glm::vec3& delta);
	void rotate(float angle, const glm::vec3& axis);

	/**
	 * Transform a local camera vector to world coordinates.
	 * 
	 * \param v
	 * \return 
	 */
	glm::vec3 getLocalVector(const glm::vec3& v) const;

	void setToPerspective(float fov, float aspect, float near_plane, float far_plane);
	void setToOrthographic(float left, float right, float bottom, float top, float near_plane, float far_plane);
	void lookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up);

	void updateViewMatrix();
	void updateProjectionMatrix();


	void setFov(float fov, bool update = false);
	void setAspect(float aspect, bool update = false);
	void setNearPlane(float near_plane, bool update = false);
	void setFarPlane(float far_plane, bool update = false);

	void setLeft(float left, bool update = false);
	void setRight(float right, bool update = false);
	void setTop(float top, bool update = false);
	void setBottom(float bottom, bool update = false);




	inline Type getType() const { return _type; }

	inline void setEye(const glm::vec3& eye) { _eye = eye; }
	inline const glm::vec3& getEye() const { return _eye; }

	inline void setCenter(const glm::vec3& center) { _center = center; }
	inline const glm::vec3& getCenter() const { return _center; }

	inline void setUp(const glm::vec3& up) { _up = up; }
	inline const glm::vec3& getUp() const { return _up; }

	inline float getFov() const { return _fov; }
	inline float getAspect() const { return _aspect; }
	inline float getNearPlane() const { return _nearPlane; }
	inline float getFarPlane() const { return _farPlane; }

	inline float getLeft() const { return _left; }
	inline float getRight() const { return _right; }
	inline float getTop() const { return _top; }
	inline float getBottom() const { return _bottom; }

	inline const glm::mat4& getViewMatrix() const { return _viewMatrix; }
	inline const glm::mat4& getProjectionMatrix() const { return _projectionMatrix; }
	inline const glm::mat4& getViewprojectionMatrix() const { return _viewprojectionMatrix; }

	inline glm::mat4 mvp(const glm::mat4& model) { return model * _viewprojectionMatrix; }
};

inline glm::mat4 operator* (const Camera& cam, const glm::mat4& model) { return model * cam.getViewprojectionMatrix(); }
inline glm::mat4 operator* (const glm::mat4& model, const Camera& cam) { return model * cam.getViewprojectionMatrix(); }
