#include "camera.h"

#include <iostream>

#include "utils/shader_constants.h"
#include "basics.h"

#include "lua/module.h"
#include "utils/lualib_constants.h"


Camera::Camera() :
	_type(),
	_eye(0, 0, 0),
	_center(0, 0, 1),
	_up(0, 1, 0),
	_lockedUp(true),
	_fov(0),
	_aspect(0),
	_nearPlane(0),
	_farPlane(0),
	_left(0),
	_right(0),
	_top(0),
	_bottom(0),
	_viewMatrix(glm::identity<glm::mat4>()),
	_projectionMatrix(glm::identity<glm::mat4>()),
	_viewprojectionMatrix(glm::identity<glm::mat4>()),
	_eulerAngles(),
	_updateEulerAngles(true),
	_frustum(),
	_updateFrustum(true)
{}

void Camera::setToGL()
{
	updateViewMatrix();
	updateProjectionMatrix();

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&_viewMatrix);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&_projectionMatrix);
	glMatrixMode(GL_MODELVIEW);
}

void Camera::move(const glm::vec3& delta)
{
	auto localDelta = getLocalVector(delta);
	_eye -= localDelta;
	_center -= localDelta;
	updateViewMatrix();
}

void Camera::rotate(float angle, const glm::vec3& axis, bool enableUpRotation)
{
	angle = glm::radians(angle);
	auto localAxis = getLocalVector(axis);

	glm::vec3 front = glm::angleAxis(angle, localAxis) * (_center - _eye);
	_center = _eye + front;
	if(enableUpRotation && !_lockedUp)
		_up = glm::rotate(_up, angle, localAxis);

	updateViewMatrix();
}

void Camera::rotate(const glm::vec3& angles)
{
	glm::quat qt = glm::quat(angles);
	glm::vec3 front = qt * (_center - _eye);
	_center = _eye + front;
	_up = qt * _up;
}

void Camera::rotateUp(const glm::vec3& angles)
{

}

void Camera::setOrientation(const glm::vec3& angles)
{
	glm::quat qt = glm::quat(angles);
	glm::vec3 front = qt * glm::vec3(0, 0, -1);
	_center = _eye + front;
	_up = qt * glm::vec3(0, 1, 0);

	updateViewMatrix();
}

/**
 * Transform a local camera vector to world coordinates.
 *
 * \param v
 * \return
 */
glm::vec3 Camera::getLocalVector(const glm::vec3& v) const
{
	glm::mat4 inv = glm::inverse(_viewMatrix);
	inv[3][0] = 0;
	inv[3][1] = 0;
	inv[3][2] = 0;
	return inv * v;
}

void Camera::setToPerspective(float fov, float aspect, float near_plane, float far_plane)
{
	_type = Type::Perspective;

	_fov = fov;
	_aspect = aspect;
	_nearPlane = near_plane;
	_farPlane = far_plane;

	updateProjectionMatrix();
}

void Camera::setToOrthographic(float left, float right, float bottom, float top, float near_plane, float far_plane)
{
	_type = Type::Orthographic;

	_left = left;
	_right = right;
	_bottom = bottom;
	_top = top;
	_nearPlane = near_plane;
	_farPlane = far_plane;

	updateProjectionMatrix();
}

void Camera::lookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up)
{
	_eye = eye;
	_center = center;
	_up = up;

	updateViewMatrix();
}

void Camera::updateViewMatrix()
{
	_updateEulerAngles = true;

	if (_type == Type::Perspective)
		_viewMatrix = glm::lookAt(_eye, _center, _up);
	else
		_viewMatrix = glm::identity<glm::mat4>();

	_viewprojectionMatrix = _projectionMatrix * _viewMatrix;
}

void Camera::updateProjectionMatrix()
{
	if (_type == Type::Orthographic)
		_projectionMatrix = glm::ortho(_left, _right, _bottom, _top, _nearPlane, _farPlane);
	else
		_projectionMatrix = glm::perspective(_fov, _aspect, _nearPlane, _farPlane);

	_viewprojectionMatrix = _projectionMatrix * _viewMatrix;
}

void Camera::updateEulerAngles() const
{
	glm::extractEulerAngleXYZ(_viewMatrix, _eulerAngles.x, _eulerAngles.y, _eulerAngles.z);
	_eulerAngles = glm::degrees(_eulerAngles);
	_updateEulerAngles = false;
}


void Camera::setFov(float fov, bool update)
{
	_fov = fov;
	if (update)
		updateViewMatrix();
}

void Camera::setAspect(float aspect, bool update)
{
	_aspect = aspect;
	if (update)
		updateViewMatrix();
}

void Camera::setNearPlane(float near_plane, bool update)
{
	_nearPlane = near_plane;
	if (update)
		updateViewMatrix();
}

void Camera::setFarPlane(float far_plane, bool update)
{
	_farPlane = far_plane;
	if (update)
		updateViewMatrix();
}

void Camera::setOrthoLeft(float left, bool update)
{
	_left = left;
	if (update)
		updateViewMatrix();
}

void Camera::setOrthoRight(float right, bool update)
{
	_right = right;
	if (update)
		updateViewMatrix();
}

void Camera::setOrthoTop(float top, bool update)
{
	_top = top;
	if (update)
		updateViewMatrix();
}

void Camera::setOrthoBottom(float bottom, bool update)
{
	_bottom = bottom;
	if (update)
		updateViewMatrix();
}


void Camera::bindToShader(ShaderProgram::Ref shader) const
{
	using namespace constants::uniform::camera;

	if (shader != nullptr)
	{
		shader->use();
		shader[viewProjection()] = _viewprojectionMatrix;
		shader[viewPos()] = _eye;
	}
}

float Camera::getDistanceTo(const glm::vec3& position) const
{
	return glm::length(position - getPosition());
}

float Camera::getZDistanceTo(const glm::vec3& position) const
{
	glm::vec3 d = position - getPosition();
	glm::vec3 front = -getFront();

	return glm::dot(d, front);
}









namespace lua::lib
{
	namespace LUA_camera { static defineLuaLibraryConstructor(registerToLua, root, state); }

	void registerCameraLibToLua()
	{
		LuaLibraryManager::instance().registerLibrary(
			::lua::lib::names::camera,
			&LUA_camera::registerToLua,
			{ ::lua::lib::names::geometry });
	}
}

namespace lua::lib::LUA_camera
{
	static const glm::vec3& getEye(const Camera* self) { return self->getEye(); } // position //
	static void setEye(Camera* self, const glm::vec3& eye) { self->setEye(eye); } // position //

	static const glm::vec3& getCenter(const Camera* self) { return self->getCenter(); }
	static void setCenter(Camera* self, const glm::vec3& center) { self->setCenter(center); }

	static const glm::vec3& getUp(const Camera* self) { return self->getUp(); }
	static void setUp(Camera* self, const glm::vec3& up) { self->setUp(up); }

	static float getFov(const Camera* self) { return self->getFov(); }

	static float getAspect(const Camera* self) { return self->getAspect(); }

	static float getNearPlane(const Camera* self) { return self->getNearPlane(); }

	static float getFarPlane(const Camera* self) { return self->getFarPlane(); }

	static const glm::mat4& getViewMatrix(const Camera* self) { return self->getViewMatrix(); }

	static const glm::mat4& getProjectionMatrix(const Camera* self) { return self->getProjectionMatrix(); }

	static const glm::mat4& getViewprojectionMatrix(const Camera* self) { return self->getViewprojectionMatrix(); }

	static glm::vec3 getFront(const Camera* self) { return self->getFront(); }

	static glm::vec3 getRight(const Camera* self) { return self->getRight(); }

	static const glm::vec3& getEulerAngles(const Camera* self) { return self->getEulerAngles(); }


	static void rotate(Camera* self, float angle, const glm::vec3& axis) { self->rotate(angle, axis, false); }
	static void rotateFree(Camera* self, float angle, const glm::vec3& axis) { self->rotate(angle, axis, true); }


	static defineLuaLibraryConstructor(registerToLua, root, state)
	{
		namespace meta = ::lua::metamethod;

		auto clss = root.beginClass<Camera>("Camera");
		clss.addConstructor<void(*)()>()
			// Properties //
			.addProperty("eye", &getEye, &setEye)
			.addProperty("center", &getCenter, &setCenter)
			.addProperty("up", &getUp, &setUp)
			.addProperty("fov", &getFov)
			.addProperty("aspect", &getAspect)
			.addProperty("nearPlane", &getNearPlane)
			.addProperty("farPlane", &getFarPlane)
			.addProperty("viewMatrix", &getViewMatrix)
			.addProperty("projectionMatrix", &getProjectionMatrix)
			.addProperty("viewProjectionMatrix", &getViewprojectionMatrix)
			.addProperty("position", &getEye, &setEye)
			.addProperty("front", &getFront)
			.addProperty("right", &getRight)
			.addProperty("eulerAngles", &getEulerAngles)
			// Functions//
			.addFunction("move", &Camera::move)
			.addFunction("rotate", &rotate)
			.addFunction("rotateFree", &rotateFree)
			.addFunction("setLockedUp", &Camera::setLockedUp)
			.addFunction("isLockedUp", &Camera::isLockedUp)
			.addFunction("getLocalVector", &Camera::getLocalVector)
			.addFunction("lookAt", &Camera::lookAt)
			.addFunction("updateViewMatrix", &Camera::updateViewMatrix)
			.addFunction("updateProjectionMatrix", &Camera::updateProjectionMatrix)
			.addFunction("getDistanceTo", &Camera::getDistanceTo)
			.addFunction("getZDistanceTo", &Camera::getZDistanceTo)
			.addFunction("mvp", &Camera::mvp);

		root = clss.endClass();
		return true;
	}
}
