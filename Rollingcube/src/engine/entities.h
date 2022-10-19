#pragma once

#include <concepts>
#include <compare>
#include <iostream>
#include <map>

#include <math/glm.h>

#include "basics.h"
#include "objmodel.h"
#include "material.h"
#include "light.h"
#include "camera.h"

#include "utils/logger.h"


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
public:
	using Id = EntityId;

private:
	Id _id;

public:
	constexpr Entity() = default;
	Entity(const Entity&) = delete;
	constexpr Entity(Entity&&) noexcept = default;
	virtual constexpr ~Entity() = default;

	Entity& operator= (const Entity&) = delete;
	Entity& operator= (Entity&&) noexcept = delete;

	constexpr bool operator== (const Entity& right) const { return _id == right._id; };

public:
	constexpr Id getEntityId() const { return _id; }

	virtual constexpr void render(const Camera& cam) override {}
	virtual constexpr void update(Time elapsedTime) override {}

protected:
	constexpr explicit Entity(Id id) : _id(id) {}

	constexpr void setEntityId(Id id) { _id = id; }
};




class TransformableEntity : public virtual Entity, public Transformable
{
public:
	TransformableEntity() = default;
	TransformableEntity(const TransformableEntity&) = delete;
	TransformableEntity(TransformableEntity&&) noexcept = default;
	virtual constexpr ~TransformableEntity() = default;

	TransformableEntity& operator= (const TransformableEntity&) = delete;
	TransformableEntity& operator= (TransformableEntity&&) noexcept = delete;
};





class ModeledEntity : public TransformableEntity
{
private:
	std::shared_ptr<ObjModel> _objModel = nullptr;
	CullSphere _cullSphere;
	Material _material;
	StaticLightContainer _staticLightContainer;
	std::shared_ptr<StaticLightManager> _staticLightManager = nullptr;
	bool _transparency = false;

	mutable ShaderProgram::Ref _lightningShader = nullptr;

public:
	ModeledEntity() = default;
	ModeledEntity(const ModeledEntity&) = delete;
	ModeledEntity(ModeledEntity&&) noexcept = default;
	virtual ~ModeledEntity();

	ModeledEntity& operator= (const ModeledEntity&) = delete;
	ModeledEntity& operator= (ModeledEntity&&) noexcept = delete;

public:
	virtual void update(Time elapsedTime);

	void renderDefault(const Camera& cam);

	void linkStaticLightManager(const std::shared_ptr<StaticLightManager>& lightManager);

public:
	virtual inline void render(const Camera& cam) { renderDefault(cam); }

	constexpr void setCullSphere(const CullSphere& cullSphere) { _cullSphere = cullSphere; }
	constexpr CullSphere& getCullSphere() { return _cullSphere; }
	constexpr const CullSphere& getCullSphere() const { return _cullSphere; }

	constexpr void setForceTransparency(bool flag) { _transparency = flag; }
	constexpr bool isForceTransparencyEnabled() const { return _transparency; }

	constexpr bool hasTransparency() const { return _transparency || _material.hasTransparency(); }

	inline bool isVisibleInCamera(const Camera& cam) const { return cam.isVisible(_cullSphere); }

	inline void setObjectModel(const std::shared_ptr<ObjModel>& objModel) { _objModel = objModel; }
	inline const std::shared_ptr<ObjModel>& getObjectModel() const { return _objModel; }

	inline void setMaterial(const Material& material) { _material = material; }
	inline const Material& getMaterial() const { return _material; }
	inline Material& getMaterial() { return _material; }

	inline const StaticLightContainer& getStaticLightContainer() { return _staticLightContainer; }

protected:
	inline ShaderProgram::Ref getLightningShader() const
	{
		if (!_lightningShader)
			_lightningShader = ShaderProgramManager::instance().getLightningShaderProgram();
		return _lightningShader;
	}
};



template <std::derived_from<ModeledEntity> _EntityTy>
class EntityCameraDistanceWrapper : public Renderable
{
public:
	using BaseEntityType = _EntityTy;

protected:
	std::shared_ptr<BaseEntityType> _entity = nullptr;
	float _distance = 0;

public:
	EntityCameraDistanceWrapper() = default;
	EntityCameraDistanceWrapper(const EntityCameraDistanceWrapper&) = default;
	EntityCameraDistanceWrapper(EntityCameraDistanceWrapper&&) noexcept = default;
	~EntityCameraDistanceWrapper() = default;

	EntityCameraDistanceWrapper& operator= (const EntityCameraDistanceWrapper&) = default;
	EntityCameraDistanceWrapper& operator= (EntityCameraDistanceWrapper&&) noexcept = default;

	constexpr bool operator== (const EntityCameraDistanceWrapper& right) const noexcept { return _distance == right._distance; }
	constexpr auto operator<=> (const EntityCameraDistanceWrapper& right) const noexcept { return _distance <=> right._distance; }

public:
	constexpr void setEntity(const std::shared_ptr<BaseEntityType>& entity) { _entity = entity; }
	constexpr const std::shared_ptr<BaseEntityType>& getEntity() const { return _entity; }

	constexpr void setDistance(float distance) { _distance = distance; }
	constexpr float getDistance() const { return _distance; }

	void render(const Camera& cam)
	{
		if (_entity != nullptr && _entity->isVisibleInCamera(cam))
			_entity->render(cam);
	}
};



template <std::derived_from<ModeledEntity> _EntityTy>
class EntityCameraDistanceCollection : public Renderable
{
public:
	using Wrapper = EntityCameraDistanceWrapper<_EntityTy>;
	using BaseEntityType = Wrapper::BaseEntityType;

	using Container = std::vector<Wrapper>;
	using SizeType = Container::size_type;
	using iterator = Container::iterator;
	using const_iterator = Container::const_iterator;
	using reverse_iterator = Container::reverse_iterator;
	using const_reverse_iterator = Container::const_reverse_iterator;

private:
	mutable Container _entities = {};

	mutable bool _sorted = false;

public:
	EntityCameraDistanceCollection() = default;
	EntityCameraDistanceCollection(const EntityCameraDistanceCollection&) = default;
	EntityCameraDistanceCollection(EntityCameraDistanceCollection&&) noexcept = default;
	~EntityCameraDistanceCollection() = default;

	EntityCameraDistanceCollection& operator= (const EntityCameraDistanceCollection&) = default;
	EntityCameraDistanceCollection& operator= (EntityCameraDistanceCollection&&) noexcept = default;

public:
	inline bool empty() const { return _entities.empty(); }
	inline SizeType size() const { return _entities.size(); }

	inline void clear() { _entities.clear(); }

	inline void addWrappedEntity(const Wrapper& wrappedEntity)
	{
		_sorted = false;
		float distance = wrappedEntity.getDistance();
		_entities.push_back(wrappedEntity);
	}

	inline void addWrappedEntity(Wrapper&& wrappedEntity)
	{
		_sorted = false;
		float distance = wrappedEntity.getDistance();
		_entities.push_back(std::move(wrappedEntity));
	}

	inline void addEntity(const std::shared_ptr<BaseEntityType>& entity, float distance)
	{
		Wrapper w;
		w.setEntity(entity);
		w.setDistance(distance);
		addWrappedEntity(std::move(w));
	}

	inline void addEntity(const std::shared_ptr<BaseEntityType>& entity, const Camera& cam)
	{
		Wrapper w;
		w.setEntity(entity);
		w.setDistance(cam.getZDistanceTo(entity->getPosition()));
		addWrappedEntity(std::move(w));
	}

	inline void sort() const
	{
		std::sort(_entities.begin(), _entities.end());
		_sorted = true;
	}

	inline void render(const Camera& cam) override
	{
		if (!_sorted)
			sort();

		for (auto& e : _entities)
			e.render(cam);
	}


public:
	inline iterator begin() noexcept { return _entities.begin(); }
	inline const_iterator begin() const noexcept { return _entities.begin(); }
	inline const_iterator cbegin() const noexcept { return _entities.cbegin(); }
	inline iterator end() noexcept { return _entities.end(); }
	inline const_iterator end() const noexcept { return _entities.end(); }
	inline const_iterator cend() const noexcept { return _entities.cend(); }

	inline reverse_iterator rbegin() noexcept { return _entities.rbegin(); }
	inline const_reverse_iterator rbegin() const noexcept { return _entities.rbegin(); }
	inline const_reverse_iterator crbegin() const noexcept { return _entities.crbegin(); }
	inline reverse_iterator rend() noexcept { return _entities.rend(); }
	inline const_reverse_iterator rend() const noexcept { return _entities.rend(); }
	inline const_reverse_iterator crend() const noexcept { return _entities.crend(); }
};
