#include "game_controller.h"


GameController GameController::Instance = GameController();

GameController::GameController()
{

}

GameController::~GameController()
{

}

void GameController::start(const std::function<bool()>& initiatingFunction, const std::function<void()>& finalizingFunction)
{
	if (_state == State::Stop)
	{
		_state = State::Starting;

		window::createMainWindow();
		if (initiatingFunction())
			loop();
		else
			_state = State::Finalizing;
		finalize(finalizingFunction);
	}
}

void GameController::loop()
{
	if (_state == State::Starting)
	{
		_state = State::Running;
		while (_state == State::Running)
		{
			update();
			render();
			dispatchEvents();

			if (glfwWindowShouldClose(window::getMainWindow()))
				stop();
		}
	}
}

void GameController::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (const auto& e : _entities)
		e.second->render(_mainCamera);

	glfwSwapBuffers(window::getMainWindow());
}

void GameController::update()
{
	_prevElapsedTime = _elapsedTime;
	_elapsedTime = _time.update();

	for (const auto& e : _entities)
		e.second->update(_elapsedTime);
}

void GameController::dispatchEvents()
{
	InputManager::dispatchEvents([this](const InputEvent& event) {
		for (const auto& e : _entities)
			e.second->dispatchEvent(event);
	});
}

void GameController::finalize(const std::function<void()>& finalizingFunction)
{
	if (_state == State::Finalizing)
	{
		finalizingFunction();
		gl::terminate();
		_state = State::Stop;
	}
}

void GameController::addEntity(const std::shared_ptr<Entity>& entity)
{
	if (entity != nullptr && !containsEntity(entity))
	{
		_entities.insert({ entity->getEntityId(), entity });
	}
}

bool GameController::removeEntity(Entity::Id id)
{
	const auto& it = _entities.find(id);
	if (it == _entities.end())
		return false;

	_entities.erase(it);
	return true;
}
