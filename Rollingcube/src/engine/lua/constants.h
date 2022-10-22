#pragma once

#include <string_view>


namespace lua::metamethod
{
	constexpr const char add[] = "__add";
	constexpr const char sub[] = "__sub";
	constexpr const char mul[] = "__mul";
	constexpr const char div[] = "__div";
	constexpr const char mod[] = "__mod";
	constexpr const char pow[] = "__pow";
	constexpr const char unm[] = "__unm";
	constexpr const char idiv[] = "__idiv";
	constexpr const char band[] = "__band";
	constexpr const char bor[] = "__bor";
	constexpr const char bxor[] = "__bxor";
	constexpr const char bnot[] = "__bnot";
	constexpr const char shl[] = "__shl";
	constexpr const char shr[] = "__shr";
	constexpr const char concat[] = "__concat";
	constexpr const char len[] = "__len";
	constexpr const char eq[] = "__eq";
	constexpr const char lt[] = "__lt";
	constexpr const char le[] = "__le";
	constexpr const char index[] = "__index";
	constexpr const char newindex[] = "__newindex";
	constexpr const char call[] = "__call";
	constexpr const char tostring[] = "__tostring";
	constexpr const char gc[] = "__gc";
	constexpr const char close[] = "__close";
	constexpr const char mode[] = "__mode";
	constexpr const char name[] = "__name";
}

namespace lua::lib::constants
{
	constexpr const char import[] = "import";
	constexpr const char openlib[] = "openlib";
}

namespace lua::lib::constants
{
	constexpr const char xpcall[] = "xpcall";
	constexpr const char select[] = "select";
	constexpr const char utf8[] = "utf8";
	constexpr const char next[] = "next";
	constexpr const char base[] = "base";
	constexpr const char load[] = "load";
	constexpr const char getmetatable[] = "getmetatable";
	constexpr const char tonumber[] = "tonumber";
	constexpr const char string[] = "string";
	constexpr const char math[] = "math";
	constexpr const char require[] = "require";
	constexpr const char _VERSION[] = "_VERSION";
	constexpr const char rawget[] = "rawget";
	constexpr const char rawset[] = "rawset";
	constexpr const char rawequal[] = "rawequal";
	constexpr const char _G[] = "_G";
	constexpr const char rawlen[] = "rawlen";
	constexpr const char error[] = "error";
	constexpr const char tostring[] = "tostring";
	constexpr const char table[] = "table";
	constexpr const char collectgarbage[] = "collectgarbage";
	constexpr const char warn[] = "warn";
	constexpr const char package[] = "package";
	constexpr const char io[] = "io";
	constexpr const char pcall[] = "pcall";
	constexpr const char loadfile[] = "loadfile";
	constexpr const char debug[] = "debug";
	constexpr const char ipairs[] = "ipairs";
	constexpr const char setmetatable[] = "setmetatable";
	constexpr const char coroutine[] = "coroutine";
	constexpr const char type[] = "type";
	constexpr const char assert[] = "assert";
	constexpr const char dofile[] = "dofile";
	constexpr const char os[] = "os";
	constexpr const char print[] = "print";
	constexpr const char pairs[] = "pairs";
}
