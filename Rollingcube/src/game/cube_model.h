#pragma once

#include <string_view>
#include <memory>

#include "engine/model.h"
#include "math/glm.h"


enum class CubeSideId
{
	Front = 0,
	Back = 1,
	Left = 2,
	Right = 3,
	Top = 4,
	Bottom = 5
};

namespace cubes::side
{
	using Id = CubeSideId;

	constexpr int count = 6;

	constexpr Id ids[count] = { Id::Front, Id::Back, Id::Left, Id::Right, Id::Top, Id::Bottom };

	constexpr std::string_view names[count] = {
		"front", "back", "left", "right", "top", "bottom"
	};

	constexpr int idToInt(Id id) { return static_cast<int>(id); }
	constexpr Id intToId(int intId) { return static_cast<Id>(intId); }

	constexpr std::string_view name(Id id) { return names[idToInt(id)]; }


	constexpr float size = 1;
	constexpr float midsize = size / 2;

	const glm::vec3& getDefaultMiddlePosition(side::Id sideId);
	const glm::vec3& getNormal(side::Id sideId);
	const glm::vec3& getRotationAngles(side::Id sideId);
}


namespace cubes::model
{
	constexpr std::string_view name = "__internal__CUBE";

	constexpr int meshId(side::Id sideId) { return side::idToInt(sideId); }
	constexpr std::string_view meshName(side::Id sideId) { return side::name(sideId); }

	Model::Ref getModel();

	inline const Mesh& getMesh(side::Id sideId) { return getModel()->getMesh(static_cast<std::size_t>(meshId(sideId))); }
	inline const Mesh& getMesh(std::string_view sideName) { return getModel()->getMesh(sideName); }

	inline void render(side::Id sideId) { getMesh(sideId).render(); }
	inline void render(std::string_view sideName) { getMesh(sideName).render(); }
	inline void render() { getModel()->render(); }
}


namespace cubes
{
	constexpr float gravity = side::size * 64;
	constexpr float baseMaxFallSpeed = side::size * 30;
}
