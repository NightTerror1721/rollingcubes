#pragma once

#include <concepts>
#include <compare>
#include <iostream>
#include <map>

#include <math/glm.h>

#include "basics.h"
#include "model.h"
#include "material.h"
#include "light.h"
#include "camera.h"
#include "bounding.h"

#include "utils/logger.h"


namespace lua::lib { void registerEntitiesLibToLua(); }


class EntityIdFactory;

class EntityId
{
public:
	using IntegerType = std::uint64_t;

	friend EntityIdFactory;

private:
	static constexpr IntegerType invalid_id = 0;

private:
	IntegerType _id = invalid_id;

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
	constexpr explicit EntityId(IntegerType id) noexcept : _id(id) {}

public:
	constexpr operator bool() const noexcept { return _id != invalid_id; }
	constexpr bool operator! () const noexcept { return _id == invalid_id; }

	constexpr explicit operator IntegerType() const noexcept { return _id; }

	friend std::ostream& operator<< (std::ostream& left, EntityId right) { return left << right._id; }
	friend std::istream& operator>> (std::istream& left, EntityId& right) { return left >> right._id; }

public:
	friend std::hash<EntityId>;
};

namespace std
{
	template <>
	struct hash<EntityId> : public hash<EntityId::IntegerType>
	{
		inline std::size_t operator() (const EntityId& id) const
		{
			return hash<EntityId::IntegerType>::_Do_hash(id._id);
		}
	};
}


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



class StaticLightSensor
{
private:
	StaticLightContainer _staticLightContainer;
	std::shared_ptr<StaticLightManager> _staticLightManager = nullptr;

public:
	StaticLightSensor() = default;
	StaticLightSensor(const StaticLightSensor&) = default;
	StaticLightSensor(StaticLightSensor&&) noexcept = default;
	virtual ~StaticLightSensor() = default;

	StaticLightSensor& operator= (const StaticLightSensor&) = default;
	StaticLightSensor& operator= (StaticLightSensor&&) noexcept = default;

public:
	inline StaticLightContainer& getStaticLightContainer() { return _staticLightContainer; }
	inline const StaticLightContainer& getStaticLightContainer() const { return _staticLightContainer; }

	inline const std::shared_ptr<StaticLightManager>& getStaticLightManager() const { return _staticLightManager; }

	inline bool hasStaticLightManagerLinked() const { return _staticLightManager != nullptr; }

	inline void linkStaticLightManager(const std::shared_ptr<StaticLightManager>& lightManager)
	{
		_staticLightManager = lightManager;
		_staticLightContainer.link(lightManager);
	}
};



class Entity : public Renderable, public Updatable, public EventDispatcher
{
public:
	using Id = EntityId;

private:
	static inline EntityIdFactory IdFactory = {};

private:
	Id _id = IdFactory();

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
	virtual constexpr void dispatchEvent(const InputEvent& event) override {}
};




class TransformableEntity : public Entity, public Transformable
{
public:
	TransformableEntity() = default;
	TransformableEntity(const TransformableEntity&) = delete;
	TransformableEntity(TransformableEntity&&) noexcept = default;
	virtual constexpr ~TransformableEntity() = default;

	TransformableEntity& operator= (const TransformableEntity&) = delete;
	TransformableEntity& operator= (TransformableEntity&&) noexcept = delete;

public:
	virtual constexpr void update(Time elapsedTime) { Transformable::update(elapsedTime); }
};





class ModelableEntity : public TransformableEntity, public StaticLightSensor
{
protected:
	bool _transparency = false;

private:
	BoundingVolumeType _boundingType = BoundingVolumeType::Sphere;
	mutable std::unique_ptr<BoundingVolume> _boundingVolume = nullptr;
	mutable bool _updateBoundingVolume = true;

	mutable ShaderProgram::Ref _lightningShader = nullptr;

public:
	ModelableEntity() = default;
	ModelableEntity(const ModelableEntity&) = delete;
	ModelableEntity(ModelableEntity&&) noexcept = default;
	virtual ~ModelableEntity() = default;

	ModelableEntity& operator= (const ModelableEntity&) = delete;
	ModelableEntity& operator= (ModelableEntity&&) noexcept = delete;

public:
	virtual void update(Time elapsedTime) override;

	void renderWithLightningShader(const Camera& cam);

public:
	virtual inline void render(const Camera& cam) { renderWithLightningShader(cam); }

	constexpr void setForceTransparency(bool flag) { _transparency = flag; }
	constexpr bool isForceTransparencyEnabled() const { return _transparency; }

	inline bool hasTransparency() const
	{
		if (_transparency)
			return true;

		Material::ConstRef mat = internalGetMaterial();
		return mat != nullptr && mat->hasTransparency();
	}

	inline void setBoundingType(BoundingVolumeType type)
	{
		_boundingType = type;
		_boundingVolume = nullptr;
		_updateBoundingVolume = true;
	}
	constexpr BoundingVolumeType getBoundingType() const { return _boundingType; }

	inline const BoundingVolume& getBoundingVolume() const
	{
		if (_updateBoundingVolume || _boundingVolume == nullptr)
			updateBoundingVolume();
		return *_boundingVolume;
	}

	inline bool isVisibleInCamera(const Camera& cam) const { return cam.isVisible(getBoundingVolume(), *this); }

	inline Model::Ref getModel() const { return internalGetModel(); }

	inline Material::ConstRef getMaterial() const { return internalGetMaterial(); }

	inline void renderBoundingVolume(const Camera& cam) const
	{
		getBoundingVolume().render(cam, *this);
	}

protected:
	virtual Model::Ref internalGetModel() const { return nullptr; };
	virtual Material::ConstRef internalGetMaterial() const { return nullptr; };

protected:
	void bindLightnigShaderRenderData(const Camera& cam) const;
	void unbindLightnigShaderRenderData() const;

	inline ShaderProgram::Ref getLightningShader() const
	{
		if (!_lightningShader)
			_lightningShader = ShaderProgramManager::instance().getLightningShaderProgram();
		return _lightningShader;
	}

private:
	inline void updateBoundingVolume() const
	{
		Model::Ref model = internalGetModel();
		if (model != nullptr)
		{
			if (_boundingVolume == nullptr)
				_boundingVolume = BoundingVolume::createUniqueFromType(_boundingType);
			if (_boundingVolume != nullptr)
				_boundingVolume->extract(*model);
			_updateBoundingVolume = false;
		}
	}

public:
	static void bindLightnigShaderRenderData(
		const Camera& cam,
		const Transformable& transform,
		Material::ConstRef material,
		ConstReference<StaticLightContainer> staticLightContainer
	);
	static void unbindLightnigShaderRenderData(Material::ConstRef material);
};





class ModelEntity : public ModelableEntity
{
protected:
	Model::Ref _model = nullptr;
	Material _material = {};

public:
	ModelEntity() = default;
	ModelEntity(const ModelEntity&) = delete;
	ModelEntity(ModelEntity&&) noexcept = default;
	virtual ~ModelEntity() = default;

	ModelEntity& operator= (const ModelEntity&) = delete;
	ModelEntity& operator= (ModelEntity&&) noexcept = delete;

public:
	inline void setModel(Model::Ref objModel) { _model = objModel; }

	inline void setMaterial(const Material& material) { _material = material; }
	inline Material& getMaterial() { return _material; }

protected:
	Model::Ref internalGetModel() const override { return _model; }
	Material::ConstRef internalGetMaterial() const override { return std::addressof(_material); }
};





template <std::derived_from<ModelableEntity> _EntityTy>
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

	void renderWithBoundingVolume(const Camera& cam)
	{
		if (_entity != nullptr && _entity->isVisibleInCamera(cam))
		{
			_entity->render(cam);
			_entity->renderBoundingVolume(cam);
		}
	}
};



template <std::derived_from<ModelableEntity> _EntityTy>
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

	bool _renderBoundings = false;

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

	inline void setRenderBoundings(bool flag) { _renderBoundings = flag; }
	inline bool isRenderBoundingsEnabled() const { return _renderBoundings; }

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

		if (!_renderBoundings)
		{
			for (auto& e : _entities)
				e.render(cam);
		}
		else
		{
			for (auto& e : _entities)
				e.renderWithBoundingVolume(cam);
		}
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
