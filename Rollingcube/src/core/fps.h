#pragma once

#include "window.h"
#include "time.h"


class TimeController
{
private:
	static constexpr Time sec_mark = Time::seconds(1);

	Clock _frameClock;
	Clock _secClock;
	Time _elapsedTime;
	unsigned int _nbFrames;
	double _fps;

public:
	TimeController(const TimeController&) = default;
	TimeController(TimeController&&) noexcept = default;
	~TimeController() = default;

	TimeController& operator= (const TimeController&) = default;
	TimeController& operator= (TimeController&&) noexcept = default;


	TimeController();


	Time update();


	inline Time getElapsedTime() const { return _elapsedTime; }
	inline double getHighPrecisionFPS() const { return _fps; }
	inline unsigned int getFPS() const { return static_cast<unsigned int>(_fps + 0.5); }
};
