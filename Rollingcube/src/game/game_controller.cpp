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
		if (init(initiatingFunction))
			loop();
		else
			_state = State::Finalizing;
		finalize(finalizingFunction);
	}
}

bool GameController::init(const std::function<bool()>& initiatingFunction)
{
	window::createMainWindow({ int(_props->windowWidth), int(_props->windowHeight) });

	return initiatingFunction();
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

	_level.render(_mainCamera);

	glfwSwapBuffers(window::getMainWindow());
}

void GameController::update()
{
	_prevElapsedTime = _elapsedTime;
	_elapsedTime = _time.update();

	_freecam.update(_elapsedTime);
	_level.update(_elapsedTime);
}

void GameController::dispatchEvents()
{
	InputManager::dispatchEvents([this](const InputEvent& event) {
		_freecam.dispatchEvent(event);
		_level.dispatchEvent(event);

		if (_stopOnEscape && event.type == InputEvent::Type::KeyPressed && event.key.key == Key::Escape)
			stop();
	});
	Mouse::setPositionToCenter();
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
