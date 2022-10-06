#include "json.h"


JsonValue json::read(std::istream& is)
{
	JsonValue json;
	is >> json;
	return json;
}

JsonValue json::read(std::string_view filepath)
{
	std::ifstream is(filepath.data());
	return read(is);
}

JsonValue json::parse(std::string_view jsoncode)
{
	return JsonValue::parse(jsoncode);
}

void json::write(std::ostream& os, const JsonValue& json)
{
	os << json;
}
void json::write(std::string_view filepath, const JsonValue& json)
{
	std::ofstream os(filepath.data());
	write(os, json);
}
