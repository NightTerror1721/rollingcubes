#include "sampler.h"

bool Sampler::create(bool repeat)
{
	if (isCreated())
		return false;

	glGenSamplers(1, &_id);
	setRepeat(repeat);
	return true;
}

void Sampler::destroy()
{
	if (isCreated())
		glDeleteSamplers(1, &_id);

	_id = 0;
}
