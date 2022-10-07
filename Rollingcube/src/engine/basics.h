#pragma once

#include "core/gl.h"
#include "core/time.h"
#include "math/glm.h"


struct Renderable
{
	virtual void render(GLenum mode = GL_TRIANGLES) = 0;
};


struct Updatable
{
	virtual void update(Time elapsedTime) = 0;
};


struct EventDispatcher
{
	//virtual void dispatchEvent(const Event& event) = 0;
};


class Transformable
{
private:
	glm::vec3 _position = { 0, 0, 0 };
	glm::vec3 _rotation = { 0, 0, 0 };
	glm::vec3 _scale = { 1, 1, 1 };

	bool _alteredFlag = true;

	mutable glm::mat4 _model = {};
	mutable bool _modelNeedUpdate = true;

	mutable glm::mat4 _invertedModel = {};
	mutable bool _invertedModelNeedUpdate = true;

public:
	Transformable() = default;
	Transformable(const Transformable&) = default;
	Transformable(Transformable&&) noexcept = default;
	~Transformable() = default;

	Transformable& operator= (const Transformable&) = default;
	Transformable& operator= (Transformable&&) noexcept = default;

public:
	const glm::mat4& getModelMatrix() const;
	const glm::mat4& getInvertedModelMatrix() const;

public:
	inline void setPosition(const glm::vec3& position) { _position = position; _modelNeedUpdate = true; _invertedModelNeedUpdate = true; _alteredFlag = true; }
	inline const glm::vec3& getPosition() const { return _position; }

	inline void setRotation(const glm::vec3& rotation) { _rotation = rotation; _modelNeedUpdate = true; _invertedModelNeedUpdate = true; _alteredFlag = true; }
	inline const glm::vec3& getRotation() const { return _rotation; }

	inline void setScale(const glm::vec3& scale) { _scale = scale; _modelNeedUpdate = true; _invertedModelNeedUpdate = true; _alteredFlag = true; }
	inline const glm::vec3& getScale() const { return _scale; }

	inline void setPosition(float x, float y, float z) { setPosition({ x, y, z }); }
	inline void setRotation(float x, float y, float z) { setRotation({ x, y, z }); }
	inline void setScale(float x, float y, float z) { setScale({ x, y, z }); }

	inline void move(const glm::vec3& delta) { setPosition(_position + delta); }
	inline void move(float x, float y, float z) { move({ x, y, z }); }

	inline void rotate(const glm::vec3& deltaAngles) { setRotation(_rotation + deltaAngles); }
	inline void rotate(float x, float y, float z) { rotate({ x, y, z }); }

	inline void scale(const glm::vec3& delta) { setScale(_scale + delta); }
	inline void scale(float x, float y, float z) { scale({ x, y, z }); }

	inline glm::mat3 getNormalMatrix() const { return glm::mat3(glm::transpose(getInvertedModelMatrix())); }

protected:
	constexpr bool isAlteredFlagEnabled() const { return _alteredFlag; }
	constexpr void disableAlteredFlag() { _alteredFlag = false; }
};
