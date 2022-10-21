#pragma once

#include <string_view>


namespace lua::metamethod
{
	constexpr std::string_view add = "__add";
	constexpr std::string_view sub = "__sub";
	constexpr std::string_view mul = "__mul";
	constexpr std::string_view div = "__div";
	constexpr std::string_view mod = "__mod";
	constexpr std::string_view pow = "__pow";
	constexpr std::string_view unm = "__unm";
	constexpr std::string_view idiv = "__idiv";
	constexpr std::string_view band = "__band";
	constexpr std::string_view bor = "__bor";
	constexpr std::string_view bxor = "__bxor";
	constexpr std::string_view bnot = "__bnot";
	constexpr std::string_view shl = "__shl";
	constexpr std::string_view shr = "__shr";
	constexpr std::string_view concat = "__concat";
	constexpr std::string_view len = "__len";
	constexpr std::string_view eq = "__eq";
	constexpr std::string_view lt = "__lt";
	constexpr std::string_view le = "__le";
	constexpr std::string_view index = "__index";
	constexpr std::string_view newindex = "__newindex";
	constexpr std::string_view call = "__call";
	constexpr std::string_view tostring = "__tostring";
	constexpr std::string_view gc = "__gc";
	constexpr std::string_view close = "__close";
	constexpr std::string_view mode = "__mode";
	constexpr std::string_view name = "__name";
}

namespace lua::lib::constants
{
	constexpr std::string_view import = "import";
	constexpr std::string_view openlib = "openlib";
}
