#pragma once

#include "core/gl.h"
#include "core/time.h"
#include "math/glm.h"

class Camera;

struct Renderable
{
	virtual void render(const Camera& cam) = 0;
};


struct Updatable
{
	virtual void update(Time elapsedTime) = 0;
};


struct EventDispatcher
{
	//virtual void dispatchEvent(const Event& event) = 0;
};


struct AxisBase
{
	glm::vec3 front = { 0, 0, 1 };
	glm::vec3 right = { 1, 0, 0 };
	glm::vec3 up = { 0, 1, 0 };

	constexpr AxisBase() = default;
	constexpr AxisBase(const AxisBase&) = default;
	constexpr AxisBase(AxisBase&&) noexcept = default;
	constexpr ~AxisBase() = default;

	constexpr AxisBase& operator= (const AxisBase&) = default;
	constexpr AxisBase& operator= (AxisBase&&) noexcept = default;

	constexpr AxisBase(const glm::vec3& front, const glm::vec3& right, const glm::vec3& up) :
		front(front),
		right(right),
		up(up)
	{}

	constexpr void extractFromView(const glm::mat4& view)
	{
		right.x = view[0][0];
		right.y = view[1][0];
		right.z = view[2][0];
		up.x = view[0][1];
		up.y = view[1][1];
		up.z = view[2][1];
		front.x = -view[0][2];
		front.y = -view[1][2];
		front.z = -view[2][2];
	}

	constexpr AxisBase(const glm::mat4& view) :
		right(view[0][0], view[1][0], view[2][0]),
		up(view[0][1], view[1][1], view[2][1]),
		front(-view[0][2], -view[1][2], -view[2][2])
	{}
};




class VersionFlag
{
public:
	using IntegerType = std::uint64_t;

private:
	IntegerType _version = 0;

public:
	constexpr VersionFlag() noexcept = default;
	constexpr VersionFlag(const VersionFlag&) noexcept = default;
	constexpr VersionFlag(VersionFlag&&) noexcept = default;
	constexpr ~VersionFlag() noexcept = default;

	constexpr VersionFlag& operator= (const VersionFlag&) noexcept = default;
	constexpr VersionFlag& operator= (VersionFlag&&) noexcept = default;

	constexpr bool operator== (const VersionFlag&) const noexcept = default;
	constexpr auto operator<=> (const VersionFlag&) const noexcept = default;

	constexpr explicit VersionFlag(IntegerType version) : _version(version) {}

public:
	constexpr operator bool() const { return _version != 0; }
	constexpr bool operator! () const { return _version == 0; }

	template <std::integral _Ty>
	constexpr bool operator== (_Ty right) { return _version == static_cast<IntegerType>(right); }

	template <std::integral _Ty>
	constexpr auto operator<=> (_Ty right) { return _version <=> static_cast<IntegerType>(right); }

	template <std::integral _Ty>
	constexpr VersionFlag operator+ (_Ty right) const { return VersionFlag(_version + static_cast<IntegerType>(right)); }
	constexpr VersionFlag operator+ (VersionFlag right) const { return VersionFlag(_version + right._version); }

	template <std::integral _Ty>
	constexpr VersionFlag operator- (_Ty right) const { return VersionFlag(_version - static_cast<IntegerType>(right)); }
	constexpr VersionFlag operator- (VersionFlag right) const { return VersionFlag(_version - right._version); }

	template <std::integral _Ty>
	constexpr VersionFlag& operator+= (_Ty right) { return _version += static_cast<IntegerType>(right), *this; }
	constexpr VersionFlag& operator+= (VersionFlag right) { return _version += right._version, *this; }

	template <std::integral _Ty>
	constexpr VersionFlag& operator-= (_Ty right) { return _version -= static_cast<IntegerType>(right), *this; }
	constexpr VersionFlag& operator-= (VersionFlag right) { return _version -= right._version, *this; }

	constexpr VersionFlag& operator++ () { return ++_version, *this; }
	constexpr VersionFlag& operator++ (int) { VersionFlag old = *this; return ++_version, old; }

	constexpr VersionFlag& operator-- () { return --_version, * this; }
	constexpr VersionFlag& operator-- (int) { VersionFlag old = *this; return --_version, old; }
};




class Versionable : public Updatable
{
protected:
	VersionFlag _lastChangeVersion;
	VersionFlag _changeVersion;

public:
	constexpr Versionable() noexcept = default;
	constexpr Versionable(const Versionable&) noexcept = default;
	constexpr Versionable(Versionable&&) noexcept = default;
	constexpr ~Versionable() noexcept = default;

	constexpr Versionable& operator= (const Versionable&) noexcept = default;
	constexpr Versionable& operator= (Versionable&&) noexcept = default;

public:
	constexpr VersionFlag getChangeVersion() const noexcept { return _changeVersion; }

	virtual constexpr void update(Time elapsedTime) override
	{
		_lastChangeVersion = _changeVersion;
	}

protected:
	constexpr VersionFlag getLastChangeVersion() const noexcept { return _lastChangeVersion; }
	constexpr VersionFlag getCurrentChangeVersion() const noexcept { return _changeVersion; }
	constexpr bool isCurrentChangeVersionUpdated() const noexcept { return _changeVersion != _lastChangeVersion; }
};




class Transformable : public Versionable
{
private:
	glm::vec3 _position = { 0, 0, 0 };
	glm::vec3 _rotation = { 0, 0, 0 };
	glm::vec3 _scale = { 1, 1, 1 };

	mutable glm::mat4 _modelMatrix = {};
	mutable bool _modelNeedUpdate = true;

	mutable glm::mat4 _invertedModelMatrix = {};
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
	inline void setPosition(const glm::vec3& position) { _position = position; _modelNeedUpdate = true; _invertedModelNeedUpdate = true; ++_changeVersion; }
	inline const glm::vec3& getPosition() const { return _position; }

	inline void setRotation(const glm::vec3& rotation) { _rotation = rotation; _modelNeedUpdate = true; _invertedModelNeedUpdate = true; ++_changeVersion; }
	inline const glm::vec3& getRotation() const { return _rotation; }

	inline void setScale(const glm::vec3& scale) { _scale = scale; _modelNeedUpdate = true; _invertedModelNeedUpdate = true; ++_changeVersion; }
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

	inline glm::vec3 getRight() const { return getModelMatrix()[0]; }
	inline glm::vec3 getUp() const { return getModelMatrix()[1]; }
	inline glm::vec3 getForward() const { return -getModelMatrix()[2]; }
	inline glm::vec3 getFront() const { return -getModelMatrix()[2]; }
	inline glm::vec3 getBackward() const { return getModelMatrix()[2]; }

	inline glm::vec3 getGlobalPosition() const { return getModelMatrix()[3]; }

	inline glm::vec3 getGlobalScale() const { return { glm::length(getRight()), glm::length(getUp()), glm::length(getBackward()) }; }
};
