#include "fps.h"

TimeController::TimeController() :
	_frameClock(),
	_secClock(),
	_elapsedTime(),
	_nbFrames(),
	_fps()
{}


Time TimeController::update()
{
	Time currentTime = _secClock.getElapsedTime();
	_elapsedTime = _frameClock.restart();
	_nbFrames++;
	if (currentTime >=  sec_mark)
	{
		_fps = static_cast<double>(static_cast<float>(_nbFrames) / currentTime.toSeconds());

		_nbFrames = 0;
		_secClock.restart(_secClock.getStartTime() + currentTime);
	}

	return _elapsedTime;
}
