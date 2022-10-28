#pragma once

#include <core/gl.h>

#include "math/glm.h"
#include "frustum.h"
#include "shader.h"
#include "basics.h"
#include "bounding.h"


enum class CameraType
{
	Perspective,
	Orthographic
};

class Camera;

glm::mat4 operator* (const Camera& cam, const glm::mat4& model);
glm::mat4 operator* (const glm::mat4& model, const Camera& cam);

namespace lua::lib { void registerCameraLibToLua(); }


class Camera
{
public:
	using Type = CameraType;

private:
	Type _type;

	glm::vec3 _eye;
	glm::vec3 _center;
	glm::vec3 _up;

	bool _lockedUp;

	float _fov;
	float _aspect;
	float _nearPlane;
	float _farPlane;

	float _left, _right, _top, _bottom;

	glm::mat4 _viewMatrix;
	glm::mat4 _projectionMatrix;
	glm::mat4 _viewprojectionMatrix;

	mutable glm::vec3 _eulerAngles;
	mutable bool _updateEulerAngles;

	mutable Frustum _frustum;
	mutable bool _updateFrustum;

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
	void rotate(float angle, const glm::vec3& axis, bool enableUpRotation = false);
	void rotate(const glm::vec3& angles);

	void rotateUp(const glm::vec3& angles);

	void setOrientation(const glm::vec3& angles);

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

	void setOrthoLeft(float left, bool update = false);
	void setOrthoRight(float right, bool update = false);
	void setOrthoTop(float top, bool update = false);
	void setOrthoBottom(float bottom, bool update = false);

	void bindToShader(ShaderProgram::Ref shader) const;

	float getDistanceTo(const glm::vec3& position) const;
	float getZDistanceTo(const glm::vec3& position) const;

private:
	void updateEulerAngles() const;

	inline void updateFrustum() const
	{
		AxisBase abase = _viewMatrix;
		_frustum.extract(
			getPosition(),
			abase.front,
			abase.right,
			abase.up,
			_aspect,
			_fov,
			_nearPlane,
			_farPlane
		);
		_updateFrustum = true;
	}

public:
	inline Type getType() const { return _type; }

	inline void setEye(const glm::vec3& eye) { _eye = eye; }
	inline const glm::vec3& getEye() const { return _eye; }

	inline void setCenter(const glm::vec3& center) { _center = center; }
	inline const glm::vec3& getCenter() const { return _center; }

	inline void setUp(const glm::vec3& up) { _up = up; }
	inline const glm::vec3& getUp() const { return _up; }

	inline void setLockedUp(bool flag) { _lockedUp = flag; }
	inline bool isLockedUp() const { return _lockedUp; }

	inline float getFov() const { return _fov; }
	inline float getAspect() const { return _aspect; }
	inline float getNearPlane() const { return _nearPlane; }
	inline float getFarPlane() const { return _farPlane; }

	inline float getOrthoLeft() const { return _left; }
	inline float getOrthoRight() const { return _right; }
	inline float getOrthoTop() const { return _top; }
	inline float getOrthoBottom() const { return _bottom; }

	inline const glm::mat4& getViewMatrix() const { return _viewMatrix; }
	inline const glm::mat4& getProjectionMatrix() const { return _projectionMatrix; }
	inline const glm::mat4& getViewprojectionMatrix() const { return _viewprojectionMatrix; }

	inline glm::mat4 mvp(const glm::mat4& model) const { return model * _viewprojectionMatrix; }

	inline void setPosition(const glm::vec3& position) { setEye(position); }
	inline const glm::vec3& getPosition() const { return getEye(); }

	inline glm::vec3 getFront() const { return _center - _eye; }
	inline glm::vec3 getRight() const { return glm::cross(_up, getFront()); }

	inline void rotateLocal(float angle, const glm::vec3& axis, bool enableUpRotation = false) { rotate(angle, getLocalVector(axis), enableUpRotation); }

	inline void rotateVertical(float angle, bool enableUpRotation = false) { rotateLocal(angle, { 0, 1, 0 }, enableUpRotation); }
	inline void rotateHorizontal(float angle, bool enableUpRotation = false) { rotateLocal(angle, { 1, 0, 0 }, enableUpRotation); }
	inline void rotateLateral(float angle) { rotateLocal(angle, { 0, 0, 1 }, true); }

	inline const glm::vec3& getEulerAngles() const
	{
		if (_updateEulerAngles)
			updateEulerAngles();
		return _eulerAngles;
	}

	inline const Frustum& getFrustum() const
	{
		if (_updateFrustum)
			updateFrustum();
		return _frustum;
	}

	inline bool isVisible(const BoundingVolume& bv, const Transformable& transf) const { return bv.isOnFrustum(getFrustum(), transf); }
};

inline glm::mat4 operator* (const Camera& cam, const glm::mat4& model) { return model * cam.getViewprojectionMatrix(); }
inline glm::mat4 operator* (const glm::mat4& model, const Camera& cam) { return model * cam.getViewprojectionMatrix(); }
