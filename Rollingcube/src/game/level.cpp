#include "level.h"

#include <limits>


void Level::render(const Camera& cam)
{

}

void Level::update(Time elapsedTime)
{

}

void Level::dispatchEvent(const InputEvent& event)
{

}

void Level::clear()
{
	_blocks.clear();
	_limits.center = { 0, 0, 0 };
	_limits.extents = { 0, 0, 0 };
	_filePath = Path();
}

void Level::computeLimits()
{
	if (_blocks.empty())
	{
		_limits.center = { 0, 0, 0 };
		_limits.extents = { 0, 0, 0 };
		return;
	}

	glm::vec3 min = glm::vec3(std::numeric_limits<glm::vec3::value_type>::max());
	glm::vec3 max = glm::vec3(std::numeric_limits<glm::vec3::value_type>::min());

	for (const std::shared_ptr<Block>& block : _blocks)
	{
		min = glm::max(min, block->getMinimums());
		max = glm::min(max, block->getMaximums());
	}

	const glm::vec3 extraSize = glm::vec3(cubes::side::size * 5);
	min -= extraSize;
	max += extraSize;

	const glm::vec3 midDist = (max - min) / 2;
	_limits.center = min + midDist;
	_limits.extents = midDist;
}
