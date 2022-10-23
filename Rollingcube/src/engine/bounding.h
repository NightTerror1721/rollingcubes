#pragma once

#include <array>

#include "core/gl.h"
#include "core/vertex_array.h"
#include "math/glm.h"
#include "frustum.h"
#include "basics.h"
#include "model.h"
#include "shader.h"

#include "utils/logger.h"


class Camera;

enum class BoundingVolumeType
{
	Sphere,
	SquareAABB,
	AABB
};



class BoundingVolume
{
public:
	constexpr BoundingVolume() = default;
	constexpr BoundingVolume(const BoundingVolume&) = default;
	constexpr BoundingVolume(BoundingVolume&&) noexcept = default;
	constexpr ~BoundingVolume() = default;

	constexpr BoundingVolume& operator= (const BoundingVolume&) = default;
	constexpr BoundingVolume& operator= (BoundingVolume&&) noexcept = default;

public:
	virtual BoundingVolumeType type() const = 0;

	virtual bool isOnFrustum(const Frustum& frustum, const Transformable& transf) const = 0;

	virtual bool isOnOrForwardPlane(const Frustum::Plane& plane) const = 0;

	virtual void extract(const Model& model) = 0;

	virtual inline void render(const Camera& cam, const Transformable& transf) const {}

	inline bool isOnFrustum(const Frustum& frustum) const
	{
		return isOnOrForwardPlane(frustum.getLeft()) &&
			isOnOrForwardPlane(frustum.getRight()) &&
			isOnOrForwardPlane(frustum.getTop()) &&
			isOnOrForwardPlane(frustum.getBottom()) &&
			isOnOrForwardPlane(frustum.getNear()) &&
			isOnOrForwardPlane(frustum.getFar());
	}

	template <std::derived_from<BoundingVolume> _Ty>
	_Ty& cast();

	template <std::derived_from<BoundingVolume> _Ty>
	_Ty& cast() const;

public:
	static constexpr std::string_view typeName(BoundingVolumeType type) noexcept
	{
		switch (type)
		{
			case BoundingVolumeType::Sphere: return "Sphere";
			case BoundingVolumeType::SquareAABB: return "SquareAABB";
			case BoundingVolumeType::AABB: return "AABB";
			default: return "<Invalid-BoundingVolume-Type>";
		}
	}

	static inline std::unique_ptr<BoundingVolume> createUniqueFromType(BoundingVolumeType type)
	{
		return std::unique_ptr<BoundingVolume>(createFromType(type));
	}

	static inline std::shared_ptr<BoundingVolume> createSharedFromType(BoundingVolumeType type)
	{
		return std::shared_ptr<BoundingVolume>(createFromType(type));
	}

private:
	static BoundingVolume* createFromType(BoundingVolumeType type);
};



class BoundingSphere : public BoundingVolume
{
public:
	static constexpr BoundingVolumeType Type = BoundingVolumeType::Sphere;

public:
	glm::vec3 center = { 0, 0, 0 };
	float radius = 0;

public:
	constexpr BoundingSphere() = default;
	constexpr BoundingSphere(const BoundingSphere&) = default;
	constexpr BoundingSphere(BoundingSphere&&) noexcept = default;
	constexpr ~BoundingSphere() = default;

	constexpr BoundingSphere& operator= (const BoundingSphere&) = default;
	constexpr BoundingSphere& operator= (BoundingSphere&&) noexcept = default;

public:
	constexpr BoundingSphere(const glm::vec3& center, float radius) :
		BoundingVolume(),
		center(center),
		radius(radius)
	{}

public:
	inline BoundingVolumeType type() const override { return Type; }

	bool isOnFrustum(const Frustum& frustum, const Transformable& transf) const override;

	inline bool isOnOrForwardPlane(const Frustum::Plane& plane) const override
	{
		return plane.getSignedDistanceToPlane(center) > -radius;
	}

	inline bool isOnFrustum(const Frustum& frustum) const { return BoundingVolume::isOnFrustum(frustum); }

	void extract(const Model& model) override;
};



class SquareAABB : public BoundingVolume
{
public:
	static constexpr BoundingVolumeType Type = BoundingVolumeType::SquareAABB;

public:
	glm::vec3 center = { 0, 0, 0 };
	float extent = 0;

public:
	constexpr SquareAABB() = default;
	constexpr SquareAABB(const SquareAABB&) = default;
	constexpr SquareAABB(SquareAABB&&) noexcept = default;
	constexpr ~SquareAABB() = default;

	constexpr SquareAABB& operator= (const SquareAABB&) = default;
	constexpr SquareAABB& operator= (SquareAABB&&) noexcept = default;

public:
	constexpr SquareAABB(const glm::vec3& center, float extent) :
		BoundingVolume(),
		center(center),
		extent(extent)
	{}

public:
	inline BoundingVolumeType type() const override { return Type; }

	bool isOnFrustum(const Frustum& frustum, const Transformable& transf) const override;

	inline bool isOnOrForwardPlane(const Frustum::Plane& plane) const override
	{
		const float r = extent * (glm::abs(plane.normal.x) + glm::abs(plane.normal.y) + glm::abs(plane.normal.z));
		return -r <= plane.getSignedDistanceToPlane(center);
	}

	inline bool isOnFrustum(const Frustum& frustum) const { return BoundingVolume::isOnFrustum(frustum); }

	void extract(const Model& model) override;
};



class AABB : public BoundingVolume
{
public:
	static constexpr BoundingVolumeType Type = BoundingVolumeType::AABB;

public:
	glm::vec3 center = { 0, 0, 0 };
	glm::vec3 extents = { 0, 0, 0 };

private:
	mutable gl::VAO _vao;
	mutable ShaderProgram::Ref _linesShader = nullptr;

public:
	AABB() = default;
	AABB(const AABB&) = default;
	AABB(AABB&&) noexcept = default;
	~AABB() = default;

	AABB& operator= (const AABB&) = default;
	AABB& operator= (AABB&&) noexcept = default;

public:
	inline AABB(const glm::vec3& center, const glm::vec3& extents) :
		BoundingVolume(),
		center(center),
		extents(extents),
		_vao()
	{}

	inline AABB(const glm::vec3& center, float iI, float iJ, float iK) :
		BoundingVolume(),
		center(center),
		extents(iI, iJ, iK),
		_vao()
	{}

public:
	inline std::array<glm::vec3, 8> getVertices() const
	{
		return {
			glm::vec3{ center.x - extents.x, center.y - extents.y, center.z - extents.z }, // -x -y -z
			glm::vec3{ center.x + extents.x, center.y - extents.y, center.z - extents.z }, // +x -y -z
			glm::vec3{ center.x - extents.x, center.y + extents.y, center.z - extents.z }, // -x +y -z
			glm::vec3{ center.x + extents.x, center.y + extents.y, center.z - extents.z }, // +x +y -z
			glm::vec3{ center.x - extents.x, center.y - extents.y, center.z + extents.z }, // -x -y +z
			glm::vec3{ center.x + extents.x, center.y - extents.y, center.z + extents.z }, // +x -y +z
			glm::vec3{ center.x - extents.x, center.y + extents.y, center.z + extents.z }, // -x +y +z
			glm::vec3{ center.x + extents.x, center.y + extents.y, center.z + extents.z }  // +x +y +z
		};
	}

	inline std::array<glm::vec3, 24> getLinesVertices() const
	{
		const auto vertices = getVertices();
		return {
			vertices[0], vertices[1],
			vertices[1], vertices[5],
			vertices[5], vertices[4],
			vertices[4], vertices[0],

			vertices[2], vertices[3],
			vertices[3], vertices[7],
			vertices[7], vertices[6],
			vertices[6], vertices[2],

			vertices[0], vertices[2],
			vertices[1], vertices[3],
			vertices[4], vertices[6],
			vertices[5], vertices[7]
		};
	}

public:
	inline BoundingVolumeType type() const override { return Type; }

	bool isOnFrustum(const Frustum& frustum, const Transformable& transf) const override;

	inline bool isOnOrForwardPlane(const Frustum::Plane& plane) const override
	{
		const float r = extents.x * glm::abs(plane.normal.x) + extents.y * glm::abs(plane.normal.y) + extents.z * glm::abs(plane.normal.z);
		return -r <= plane.getSignedDistanceToPlane(center);
	}

	inline bool isOnFrustum(const Frustum& frustum) const { return BoundingVolume::isOnFrustum(frustum); }

	void extract(const Model& model) override;

	void render(const Camera& cam, const Transformable& transf) const override;
};



template <std::derived_from<BoundingVolume> _Ty>
inline _Ty& BoundingVolume::cast()
{
	if (type() != _Ty::Type)
		logger::error("Cannot cast {} to {}.", typeName(type()), typeName(_Ty::Type));
	return reinterpret_cast<_Ty&>(*this);
}

template <std::derived_from<BoundingVolume> _Ty>
inline _Ty& BoundingVolume::cast() const
{
	if (type() != _Ty::Type)
		logger::error("Cannot cast {} to {}.", typeName(type()), typeName(_Ty::Type));
	return reinterpret_cast<const _Ty&>(*this);
}


inline BoundingVolume* BoundingVolume::createFromType(BoundingVolumeType type)
{
	switch (type)
	{
		case BoundingVolumeType::Sphere: return new BoundingSphere;
		case BoundingVolumeType::SquareAABB: return new SquareAABB;
		case BoundingVolumeType::AABB: return new AABB;
		default: return nullptr;
	}
}
