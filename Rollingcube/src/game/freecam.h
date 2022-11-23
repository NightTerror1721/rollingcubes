#pragma once

#include "engine/camera.h"


class FreecamController
{
private:
	enum class Direction
	{
		None = 0,

		Front = 1,
		Back = -1,

		Right = 1,
		Left = -1,

		Up = 1,
		Down = -1
	};

public:
	static constexpr float RotationSpeed = 5.0f;
	static constexpr float TranslationSpeed = 4.0f;

private:
	Reference<Camera> _cam = nullptr;
	Direction _horizontalDir = Direction::None;
	Direction _verticalDir = Direction::None;
	Direction _depthDir = Direction::None;
	float _pitchDelta = 0;
	float _yawDelta = 0;
	bool _enabled = false;

public:
	FreecamController() = default;
	FreecamController(const FreecamController&) = default;
	FreecamController(FreecamController&&) noexcept = default;
	~FreecamController() = default;

	FreecamController& operator= (const FreecamController&) = default;
	FreecamController& operator= (FreecamController&&) noexcept = default;

public:
	constexpr void setEnabled(bool flag)
	{
		_enabled = flag;
		_horizontalDir = Direction::None;
		_verticalDir = Direction::None;
		_depthDir = Direction::None;
		_pitchDelta = 0;
		_yawDelta = 0;
	}
	constexpr bool isEnabled() const { return _enabled; }

public:
	void update(Time elapsedTime);
	void dispatchEvent(const InputEvent& event);
};
