#pragma once

#include <string_view>
#include <memory>

#include "engine/objmodel.h"
#include "math/glm.h"


namespace cubes::model
{
	constexpr std::size_t cube_size = 20;

	constexpr std::size_t cube_side_count = 6;

	constexpr unsigned int cube_side_id[cube_side_count] = { 0, 1, 2, 3, 4, 5 };

	constexpr std::string_view cube_side_name[cube_side_count] = {
		"up", "down", "left", "right", "front", "back"
	};

	const std::shared_ptr<ObjModel>& getModel();
}
