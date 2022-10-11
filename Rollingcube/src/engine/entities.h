#pragma once

#include <compare>
#include <iostream>

#include <math/glm.h>

#include "basics.h"
#include "objmodel.h"
#include "material.h"
#include "light.h"


class EntityIdFactory;

class EntityId
{
public:
	friend EntityIdFactory;

private:
	static constexpr std::uint64_t invalid_id = 0;

private:
	std::uint64_t _id = invalid_id;

public:
	constexpr EntityId() noexcept = default;
	constexpr EntityId(const EntityId&) noexcept = default;
	constexpr EntityId(EntityId&&) noexcept = default;
	constexpr ~EntityId() noexcept = default;

	constexpr EntityId& operator= (const EntityId&) noexcept = default;
	constexpr EntityId& operator= (EntityId&&) noexcept = default;

	constexpr bool operator== (const EntityId&) const noexcept = default;
	constexpr auto operator<=> (const EntityId&) const noexcept = default;

private:
	constexpr explicit EntityId(std::uint64_t id) noexcept : _id(id) {}

public:
	constexpr operator bool() const noexcept { return _id != invalid_id; }
	constexpr bool operator! () const noexcept { return _id == invalid_id; }

	constexpr explicit operator std::uint64_t() const noexcept { return _id; }

	friend std::ostream& operator<< (std::ostream& left, EntityId right) { return left << right._id; }
	friend std::istream& operator>> (std::istream& left, EntityId& right) { return left >> right._id; }
};


class EntityIdFactory
{
private:
	std::uint64_t _nextId = 1;

public:
	constexpr EntityIdFactory() noexcept = default;
	constexpr EntityIdFactory(const EntityIdFactory&) noexcept = default;
	constexpr EntityIdFactory(EntityIdFactory&&) noexcept = default;
	constexpr ~EntityIdFactory() noexcept = default;

	constexpr EntityIdFactory& operator= (const EntityIdFactory&) noexcept = default;
	constexpr EntityIdFactory& operator= (EntityIdFactory&&) noexcept = default;


	constexpr EntityId operator() () noexcept { return EntityId(_nextId++); }
};




class Entity : public Renderable, public Updatable, public EventDispatcher
{
private:
	EntityId _id;

public:
	Entity() = default;
	Entity(const Entity&) = delete;
	Entity(Entity&&) noexcept = default;
	virtual ~Entity() = default;

	Entity& operator= (const Entity&) = delete;
	Entity& operator= (Entity&&) noexcept = delete;

	constexpr bool operator== (const Entity& right) const { return _id == right._id; };

public:
	constexpr EntityId getEntityId() const { return _id; }

	virtual constexpr void render(GLenum mode = GL_TRIANGLES) {}
	virtual constexpr void update(Time elapsedTime) {}

protected:
	constexpr explicit Entity(EntityId id) : _id(id) {}

	constexpr void setEntityId(EntityId id) { _id = id; }
};




class TransformableEntity : public Entity, public Transformable
{
public:
	TransformableEntity() = default;
	TransformableEntity(const TransformableEntity&) = delete;
	TransformableEntity(TransformableEntity&&) noexcept = default;
	virtual ~TransformableEntity() = default;

	TransformableEntity& operator= (const TransformableEntity&) = delete;
	TransformableEntity& operator= (TransformableEntity&&) noexcept = delete;
};





class ModeledEntity : public TransformableEntity
{
private:
	std::shared_ptr<ObjModel> _objModel = nullptr;
	Material _material;
	StaticLightContainer _staticLightContainer;
	std::shared_ptr<StaticLightManager> _staticLightManager = nullptr;

	ShaderProgram::Ref _lightningShader = nullptr;

public:
	ModeledEntity() = default;
	ModeledEntity(const ModeledEntity&) = delete;
	ModeledEntity(ModeledEntity&&) noexcept = default;
	virtual ~ModeledEntity();

	ModeledEntity& operator= (const ModeledEntity&) = delete;
	ModeledEntity& operator= (ModeledEntity&&) noexcept = delete;

public:
	virtual void update(Time elapsedTime);

	void renderDefault(GLenum mode = GL_TRIANGLES);

	void linkStaticLightManager(const std::shared_ptr<StaticLightManager>& lightManager);

public:
	virtual inline void render(GLenum mode = GL_TRIANGLES) { renderDefault(mode); }

	inline void setObjectModel(const std::shared_ptr<ObjModel>& objModel) { _objModel = objModel; }
	inline const std::shared_ptr<ObjModel>& getObjectModel() const { return _objModel; }

	inline void setMaterial(const Material& material) { _material = material; }
	inline const Material& getMaterial() const { return _material; }
	inline Material& getMaterial() { return _material; }

	inline const StaticLightContainer& getStaticLightContainer() { return _staticLightContainer; }

protected:
	inline ShaderProgram::Ref getLightningShader()
	{
		if (!_lightningShader)
			_lightningShader = ShaderProgramManager::instance().getLightningShaderProgram();
		return _lightningShader;
	}
};
