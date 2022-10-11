#pragma once

#include <string_view>

#pragma warning(push)
#pragma warning(disable: 26800)
#pragma warning(disable: 26819)
#include <nlohmann/json.hpp>
#pragma warning(pop)

#include "io_utils.h"

using JsonValue = nlohmann::json;
using JsonObject = nlohmann::json::object_t;
using JsonArray = nlohmann::json::array_t;

namespace json
{
	JsonValue read(std::istream& is);
	JsonValue read(std::string_view filepath);
	JsonValue parse(std::string_view jsoncode);

	void write(std::ostream& os, const JsonValue& json);
	void write(std::string_view filepath, const JsonValue& json);
}
