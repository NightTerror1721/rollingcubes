#pragma once

#include "cube_model.h"


namespace balls
{
	constexpr float radius = cubes::side::size / 6.0f;
	constexpr float diameter = radius * 2;
	constexpr float size = diameter;

	constexpr float baseMoveSpeed = cubes::side::size * 4;
	constexpr float baseGroundDist = radius;

	constexpr float baseUpJumpSpeed = cubes::side::size * 16;
	constexpr float baseFarJumpSpeed = cubes::side::size * 18;
	constexpr float baseHighJumpSpeed = cubes::side::size * 30;

	constexpr float baseLateForwardJumpTime = 0.075f;

	constexpr float baseForwardPressTime = 0.15f;
	constexpr float baseForwardJumpPressTime = 0.1f;

	constexpr float baseRotationStopTime = 0;

	constexpr float baseLookRelaxSpeed = 3;
	constexpr float baseLookSpeed = 3;

	constexpr float baseFallTillLookDown = 0.4f;


	constexpr float baseRotationSpeed(float moveSpeed) { return 1.0f * moveSpeed; }
	constexpr float upRotationSpeed(float moveSpeed, float groundDist) { return moveSpeed * cubes::side::size / (cubes::side::size - groundDist); }
	constexpr float downRotationSpeed(float moveSpeed, float groundDist) { return moveSpeed * cubes::side::size / (cubes::side::size + groundDist); }
}

namespace balls::model
{
	constexpr std::string_view name = "__internal__BALL";

	Model::Ref getModel();
}

namespace balls
{
	inline glm::vec3 getScale()
	{
		const auto& msize = model::getModel()->getSize();
		return { balls::size / msize.x, balls::size / msize.y, balls::size / msize.z };
	}
}
