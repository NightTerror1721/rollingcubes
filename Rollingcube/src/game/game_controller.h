#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>

#include "core/gl.h"
#include "core/window.h"

#include "engine/entities.h"

#include "level.h"
#include "freecam.h"
#include "properties.h"


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

	FreecamController _freecam = {};
	Level _level;

	const Reference<Properties> _props = Properties::referenceInstance();

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
	bool init(const std::function<bool()>& initiatingFunction);

	void loop();
	void render();
	void update();
	void dispatchEvents();

	void finalize(const std::function<void()>& finalizingFunction);

public:
	constexpr bool isRunning() const { return _state == State::Running; }

	constexpr Time getElapsedTime() const { return _elapsedTime; }
	constexpr Time getPrevElapsedTime() const { return _prevElapsedTime; }

	constexpr Camera& getMainCamera() { return _mainCamera; }
	constexpr const Camera& getMainCamera() const { return _mainCamera; }

	constexpr FreecamController& getFreecam() { return _freecam; }
	constexpr const FreecamController& getFreecam() const { return _freecam; }

	constexpr Level& getLevel() { return _level; }
	constexpr const Level& getLevel() const { return _level; }

	inline void stop()
	{
		if (_state == State::Running)
			_state = State::Finalizing;
	}

public:
	static constexpr GameController& instance() { return Instance; }
};

namespace game
{
	constexpr inline GameController& get() { return GameController::instance(); }
}
