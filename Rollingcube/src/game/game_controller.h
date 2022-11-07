#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>

#include "core/gl.h"
#include "core/window.h"

#include "engine/entities.h"


enum class GameControllerState
{
	Stop,
	Running,
	Starting,
	Finalizing
};

class GameController
{
public:
	using State = GameControllerState;

private:
	static GameController Instance;

private:
	State _state = State::Stop;
	TimeController _time = {};

	Time _prevElapsedTime = {};
	Time _elapsedTime = {};

	Camera _mainCamera = {};

	std::unordered_map<Entity::Id, std::shared_ptr<Entity>> _entities = {};

private:
	GameController(const GameController&) = delete;
	GameController(GameController&&) noexcept = delete;

	GameController& operator= (const GameController&) = delete;
	GameController& operator= (GameController&&) noexcept = delete;

private:
	GameController();
	~GameController();

public:
	void start(const std::function<bool()>& initiatingFunction = {}, const std::function<void()>& finalizingFunction = {});

private:
	void loop();
	void render();
	void update();
	void dispatchEvents();

	void finalize(const std::function<void()>& finalizingFunction);

	void addEntity(const std::shared_ptr<Entity>& entity);

	bool removeEntity(Entity::Id id);

public:
	constexpr bool isRunning() const { return _state == State::Running; }

	constexpr Time getElapsedTime() const { return _elapsedTime; }
	constexpr Time getPrevElapsedTime() const { return _prevElapsedTime; }

	constexpr Camera& getMainCamera() { return _mainCamera; }
	constexpr const Camera& getMainCamera() const { return _mainCamera; }

	inline void stop()
	{
		if (_state == State::Running)
			_state = State::Finalizing;
	}

	inline bool removeEntity(const Entity& entity) { return removeEntity(entity.getEntityId()); }
	inline bool removeEntity(const std::shared_ptr<Entity>& entity)
	{
		if(entity != nullptr)
			removeEntity(entity->getEntityId());
	}

	inline bool containsEntity(Entity::Id id) const { return _entities.contains(id); }
	inline bool containsEntity(const Entity& entity) const { return _entities.contains(entity.getEntityId()); }
	inline bool containsEntity(const std::shared_ptr<Entity>& entity) const
	{
		return entity != nullptr && _entities.contains(entity->getEntityId());
	}

public:
	static constexpr GameController& instance() { return Instance; }
};

namespace game
{
	constexpr inline GameController& get() { return GameController::instance(); }
}
