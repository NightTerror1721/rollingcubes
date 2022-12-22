#include "luadebuglib.h"

#include <string>

#include "engine/camera.h"

#include "engine/lua/module.h"
#include "utils/lualib_constants.h"


namespace lua::lib
{
	namespace LUA_debuglib { static defineLuaLibraryConstructor(registerToLua, root, state); }

	void registerDebugLibToLua()
	{
		LuaLibraryManager::instance().registerLibrary(
			::lua::lib::names::debug,
			&LUA_debuglib::registerToLua,
			{}
		);
	}
}

namespace lua::lib::LUA_debuglib
{
	static std::string getClassName(LuaRef obj, lua_State* state)
	{
		obj.print(std::cout); std::cout << std::endl;
		obj.push(state);
		lua_getmetatable(state, -1);
		LuaRef metatable = LuaRef::fromStack(state, -1);
//		metatable.print(std::cout); std::cout << std::endl;
		if (metatable.isTable())
		{
			lua::utils::forEachInTable(metatable, [](const LuaRef& key, const LuaRef& value) {
				key.print(std::cout); std::cout << std::endl;
			});
		}

		return "";
	}

	static void testCam(Camera* cam, lua_State* state)
	{
		LuaRef rcam(state, cam);

	}


	static defineLuaLibraryConstructor(registerToLua, root, state)
	{
		namespace meta = lua::metamethod;

		root.addFunction("getClassName", &getClassName);
		root.addFunction("testCam", &testCam);

		return true;
	}
}
