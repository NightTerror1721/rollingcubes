#include "freecam.h"

#include "input_binds.h"


void FreecamController::update(Time elapsedTime)
{
	if (_enabled && _cam != nullptr)
	{
		const float deltaTime = elapsedTime.toSeconds();

		glm::vec3 delta = {
			deltaTime * TranslationSpeed * int(_horizontalDir),
			deltaTime * TranslationSpeed * int(_verticalDir),
			deltaTime * TranslationSpeed * int(_depthDir)
		};
		_cam->move(delta);

		if (_pitchDelta != 0)
		{
			_cam->rotate(TranslationSpeed * deltaTime * _pitchDelta, { 0, 1, 0 }, false);
			_pitchDelta = 0;
		}
		if (_yawDelta != 0)
		{
			_cam->rotate(TranslationSpeed * deltaTime * _yawDelta, { 1, 0, 0 }, false);
			_yawDelta = 0;
		}
	}
}

void FreecamController::dispatchEvent(const InputEvent& event)
{
	if (_enabled && _cam != nullptr)
	{
		switch (event.type)
		{
			case InputEvent::Type::MouseMoved:
				_pitchDelta = float(event.mouseMove.position.x);
				_yawDelta = float(event.mouseMove.position.y);
				break;

			default:
				if (event.bindState.bind == input::bind::FreecamForward)
					_depthDir = event.bindState.value != 0 ? Direction::Front : Direction::None;
				else if (event.bindState.bind == input::bind::FreecamBackward)
					_depthDir = event.bindState.value != 0 ? Direction::Back : Direction::None;

				if (event.bindState.bind == input::bind::FreecamRight)
					_horizontalDir = event.bindState.value != 0 ? Direction::Right : Direction::None;
				else if (event.bindState.bind == input::bind::FreecamLeft)
					_horizontalDir = event.bindState.value != 0 ? Direction::Left : Direction::None;

				break;
		}
	}
}
