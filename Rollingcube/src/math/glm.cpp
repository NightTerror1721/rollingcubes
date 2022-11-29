#include "glm.h"

#include <sstream>
#include <functional>

#include "engine/lua/module.h"
#include "utils/lualib_constants.h"


namespace lua::lib
{
	defineLuaLibraryConstructor(LUA_glmLib, root, state);

	void registerGlmToLua()
	{
		LuaLibraryManager::instance().registerLibrary(::lua::lib::names::geometry, &LUA_glmLib);
	}


	namespace LUA_vec2
	{
		static glm::vec2 constructor(LuaRef cls, LuaRef lx, LuaRef ly)
		{
			if (lx.isNil())
				return glm::vec2(0, 0);

			if (ly.isNil())
			{
				if (lx.isInstance<glm::vec2>())
					return glm::vec2(lx.cast<glm::vec2>());

				if (lx.isInstance<glm::vec3>())
					return glm::vec2(lx.cast<glm::vec3>());

				if (lx.isInstance<glm::vec4>())
					return glm::vec2(lx.cast<glm::vec4>());
			}

			return glm::vec2(lx.cast<float>(), ly.cast<float>());
		}

		static bool eq(const glm::vec2* left, const glm::vec2& right) { return (*left) == right; }

		static glm::vec2 add(const glm::vec2* left, const glm::vec2& right) { return (*left) + right; }
		static glm::vec2 sub(const glm::vec2* left, const glm::vec2& right) { return (*left) - right; }
		static glm::vec2 neg(const glm::vec2* left) { return -(*left); }

		static glm::vec2 mul(const glm::vec2* left, LuaRef right)
		{
			if (right.isNumber())
				return (*left) * right.cast<float>();
			return (*left) * right.cast<glm::vec2>();
		}
		static glm::vec2 div(const glm::vec2* left, LuaRef right)
		{
			if (right.isNumber())
				return (*left) / right.cast<float>();
			return (*left) / right.cast<glm::vec2>();
		}

		static std::string toString(const glm::vec2* left)
		{
			return std::format("({}, {})", left->x, left->y);
		}

		static int len(const glm::vec2* left) { return static_cast<int>(glm::vec2::length()); }

		static float index(const glm::vec2* left, int index)
		{
			return (*left)[index];
		}

		static float newindex(glm::vec2* left, int index, float value)
		{
			return (*left)[index] = value;
		}

		static glm::vec2 normalize(const glm::vec2* left) { return glm::normalize(*left); }
		static float length(const glm::vec2* left) { return glm::length(*left); }
		static float dot(const glm::vec2* left, const glm::vec2& right) { return glm::dot(*left, right); }
	}

	static defineLuaLibraryConstructor(LUA_glmLib_vec2, root, state)
	{
		using namespace LUA_vec2;
		namespace meta = ::lua::metamethod;

		// glm::vec2 //
		auto clss = root.beginClass<glm::vec2>("vec2");
		clss.addData("x", &glm::vec2::x)
			.addData("y", &glm::vec2::y)
			.addData("r", &glm::vec2::r)
			.addData("g", &glm::vec2::g)
			.addData("s", &glm::vec2::s)
			.addData("t", &glm::vec2::t)
			.addStaticFunction(meta::call, &constructor)
			.addFunction(meta::eq, &eq)
			.addFunction(meta::add, &add)
			.addFunction(meta::sub, &sub)
			.addFunction(meta::mul, &mul)
			.addFunction(meta::div, &div)
			.addFunction(meta::unm, &neg)
			.addFunction(meta::len, &len)
			.addFunction(meta::tostring, &toString)
			.addFunction(meta::index, &index)
			.addFunction(meta::newindex, &newindex)
			.addFunction("normalize", &normalize)
			.addFunction("length", &length)
			.addFunction("dot", &dot)
			;
		root = clss.endClass();

		return true;
	}
	


	namespace LUA_vec3
	{
		static glm::vec3 constructor(LuaRef cls, LuaRef lx, LuaRef ly, LuaRef lz)
		{
			if (lx.isNil())
				return glm::vec3(0, 0, 0);

			if (ly.isNil())
			{
				if(lx.isInstance<glm::vec3>())
					return glm::vec3(lx.cast<glm::vec3>());

				if (lx.isInstance<glm::vec4>())
					return glm::vec3(lx.cast<glm::vec4>());
			}

			if (lz.isNil())
			{
				if (lx.isInstance<glm::vec2>())
					return glm::vec3(lx.cast<glm::vec2>(), ly.cast<float>());
			}

			return glm::vec3(lx.cast<float>(), ly.cast<float>(), lz.cast<float>());
		}

		static bool eq(const glm::vec3* left, const glm::vec3& right) { return (*left) == right; }

		static glm::vec3 add(const glm::vec3* left, const glm::vec3& right) { return (*left) + right; }
		static glm::vec3 sub(const glm::vec3* left, const glm::vec3& right) { return (*left) - right; }
		static glm::vec3 neg(const glm::vec3* left) { return -(*left); }

		static glm::vec3 mul(const glm::vec3* left, LuaRef right)
		{
			if (right.isNumber())
				return (*left) * right.cast<float>();
			return (*left) * right.cast<glm::vec3>();
		}
		static glm::vec3 div(const glm::vec3* left, LuaRef right)
		{
			if (right.isNumber())
				return (*left) / right.cast<float>();
			return (*left) / right.cast<glm::vec3>();
		}

		static std::string toString(const glm::vec3* left)
		{
			return std::format("({}, {}, {})", left->x, left->y, left->z);
		}

		static int len(const glm::vec3* left) { return static_cast<int>(glm::vec3::length()); }

		static float index(const glm::vec3* left, int index)
		{
			return (*left)[index];
		}

		static float newindex(glm::vec3* left, int index, float value)
		{
			return (*left)[index] = value;
		}

		static glm::vec3 normalize(const glm::vec3* left) { return glm::normalize(*left); }
		static float length(const glm::vec3* left) { return glm::length(*left); }
		static float dot(const glm::vec3* left, const glm::vec3& right) { return glm::dot(*left, right); }
		static glm::vec3 cross(const glm::vec3* left, const glm::vec3& right) { return glm::cross(*left, right); }
	}

	static defineLuaLibraryConstructor(LUA_glmLib_vec3, root, state)
	{
		using namespace LUA_vec3;
		namespace meta = ::lua::metamethod;

		// glm::vec3 //
		auto clss = root.beginClass<glm::vec3>("vec3");
		clss.addData("x", &glm::vec3::x)
			.addData("y", &glm::vec3::y)
			.addData("z", &glm::vec3::z)
			.addData("r", &glm::vec3::r)
			.addData("g", &glm::vec3::g)
			.addData("b", &glm::vec3::b)
			.addData("s", &glm::vec3::s)
			.addData("t", &glm::vec3::t)
			.addData("p", &glm::vec3::p)
			.addStaticFunction(meta::call, &constructor)
			.addFunction(meta::eq, &eq)
			.addFunction(meta::add, &add)
			.addFunction(meta::sub, &sub)
			.addFunction(meta::mul, &mul)
			.addFunction(meta::div, &div)
			.addFunction(meta::unm, &neg)
			.addFunction(meta::len, &len)
			.addFunction(meta::tostring, &toString)
			.addFunction(meta::index, &index)
			.addFunction(meta::newindex, &newindex)
			.addFunction("normalize", &normalize)
			.addFunction("length", &length)
			.addFunction("dot", &dot)
			.addFunction("cross", &cross)
			;
		root = clss.endClass();

		return true;
	}




	namespace LUA_vec4
	{
		static glm::vec4 constructor(LuaRef cls, LuaRef lx, LuaRef ly, LuaRef lz, LuaRef lw)
		{
			if (lx.isNil())
				return glm::vec4(0, 0, 0, 0);

			if (ly.isNil())
			{
				if (lx.isInstance<glm::vec4>())
					return glm::vec4(lx.cast<glm::vec4>());
			}

			if (lz.isNil())
			{
				if (lx.isInstance<glm::vec3>())
					return glm::vec4(lx.cast<glm::vec3>(), ly.cast<float>());
			}

			if (lw.isNil())
			{
				if (lx.isInstance<glm::vec2>())
					return glm::vec4(lx.cast<glm::vec2>(), ly.cast<float>(), lz.cast<float>());
			}

			return glm::vec4(lx.cast<float>(), ly.cast<float>(), lz.cast<float>(), lw.cast<float>());
		}

		static bool eq(const glm::vec4* left, const glm::vec4& right) { return (*left) == right; }

		static glm::vec4 add(const glm::vec4* left, const glm::vec4& right) { return (*left) + right; }
		static glm::vec4 sub(const glm::vec4* left, const glm::vec4& right) { return (*left) - right; }
		static glm::vec4 neg(const glm::vec4* left) { return -(*left); }

		static glm::vec4 mul(const glm::vec4* left, LuaRef right)
		{
			if (right.isNumber())
				return (*left) * right.cast<float>();
			return (*left) * right.cast<glm::vec4>();
		}
		static glm::vec4 div(const glm::vec4* left, LuaRef right)
		{
			if (right.isNumber())
				return (*left) / right.cast<float>();
			return (*left) / right.cast<glm::vec4>();
		}

		static std::string toString(const glm::vec4* left)
		{
			return std::format("({}, {}, {}, {})", left->x, left->y, left->z, left->w);
		}

		static int len(const glm::vec4* left) { return static_cast<int>(glm::vec4::length()); }

		static float index(const glm::vec4* left, int index)
		{
			return (*left)[index];
		}

		static float newindex(glm::vec4* left, int index, float value)
		{
			return (*left)[index] = value;
		}

		static glm::vec4 normalize(const glm::vec4* left) { return glm::normalize(*left); }
		static float length(const glm::vec4* left) { return glm::length(*left); }
		static float dot(const glm::vec4* left, const glm::vec4& right) { return glm::dot(*left, right); }
	}

	static defineLuaLibraryConstructor(LUA_glmLib_vec4, root, state)
	{
		using namespace LUA_vec4;
		namespace meta = ::lua::metamethod;

		// glm::vec4 //
		auto clss = root.beginClass<glm::vec4>("vec4");
		clss.addData("x", &glm::vec4::x)
			.addData("y", &glm::vec4::y)
			.addData("z", &glm::vec4::z)
			.addData("w", &glm::vec4::w)
			.addData("r", &glm::vec4::r)
			.addData("g", &glm::vec4::g)
			.addData("b", &glm::vec4::b)
			.addData("a", &glm::vec4::a)
			.addData("s", &glm::vec4::s)
			.addData("t", &glm::vec4::t)
			.addData("p", &glm::vec4::p)
			.addData("q", &glm::vec4::q)
			.addStaticFunction(meta::call, &constructor)
			.addFunction(meta::eq, &eq)
			.addFunction(meta::add, &add)
			.addFunction(meta::sub, &sub)
			.addFunction(meta::mul, &mul)
			.addFunction(meta::div, &div)
			.addFunction(meta::unm, &neg)
			.addFunction(meta::len, &len)
			.addFunction(meta::tostring, &toString)
			.addFunction(meta::index, &index)
			.addFunction(meta::newindex, &newindex)
			.addFunction("normalize", &normalize)
			.addFunction("length", &length)
			.addFunction("dot", &dot)
			;
		root = clss.endClass();

		return true;
	}




	namespace LUA_mat4
	{
		static glm::mat4 constructor(LuaRef cls, LuaRef arg)
		{
			if (arg.isNil())
				return glm::mat4();

			return glm::mat4(arg.cast<float>());
		}

		static glm::mat4 s_identity() { return glm::identity<glm::mat4>(); }

		static bool eq(const glm::mat4* left, const glm::mat4& right) { return (*left) == right; }

		static glm::mat4 add(const glm::mat4* left, const glm::mat4& right) { return (*left) + right; }
		static glm::mat4 sub(const glm::mat4* left, const glm::mat4& right) { return (*left) - right; }
		static glm::mat4 neg(const glm::mat4* left) { return -(*left); }

		static LuaRef mul(const glm::mat4* left, LuaRef right, lua_State* state)
		{
			if (right.isNumber())
				return LuaRef(state, (*left) * right.cast<float>());
			if (right.isInstance<glm::vec4>())
				return LuaRef(state, (*left) * right.cast<glm::vec4>());
			return LuaRef(state, (*left) * right.cast<glm::mat4>());
		}
		static glm::mat4 div(const glm::mat4* left, LuaRef right)
		{
			if (right.isNumber())
				return (*left) / right.cast<float>();
			return (*left) / right.cast<glm::mat4>();
		}

		static std::string toString(const glm::mat4* left)
		{
			std::stringstream ss;

			ss << "{";
			ss << lua::lib::LUA_vec4::toString(std::addressof((*left)[0])) << ", ";
			ss << lua::lib::LUA_vec4::toString(std::addressof((*left)[1])) << ", ";
			ss << lua::lib::LUA_vec4::toString(std::addressof((*left)[2])) << ", ";
			ss << lua::lib::LUA_vec4::toString(std::addressof((*left)[3])) << "}";

			return ss.str();
		}

		static int len(const glm::mat4* left) { return static_cast<int>(glm::mat4::length()); }

		static glm::vec4* index(glm::mat4* left, int index)
		{
			return std::addressof((*left)[index]);
		}

		static glm::vec4* newindex(glm::mat4* left, int index, const glm::vec4& value)
		{
			return std::addressof((*left)[index] = value);
		}

		static glm::mat4 transpose(const glm::mat4* self) { return glm::transpose(*self); }
		static glm::mat4 inverse(const glm::mat4* self) { return glm::inverse(*self); }

		static glm::mat4 translate(const glm::mat4* self, const glm::vec3& position) { return glm::translate(*self, position); }
		static glm::mat4 scale(const glm::mat4* self, const glm::vec3& position) { return glm::scale(*self, position); }
		static glm::mat4 rotate(const glm::mat4* self, float angle, const glm::vec3& position) { return glm::rotate(*self, angle, position); }

		static glm::mat4 s_lookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up)
		{
			return glm::lookAt(eye, center, up);
		}

		static glm::mat4 s_ortho(float left, float right, float bottom, float top, float near, float far)
		{
			return glm::ortho(left, right, bottom, top, near, far);
		}

		static glm::mat4 s_perspective(float fov, float aspect, float near, float far)
		{
			return glm::perspective(fov, aspect, near, far);
		}
	}

	static defineLuaLibraryConstructor(LUA_glmLib_mat4, root, state)
	{
		using namespace LUA_mat4;
		namespace meta = ::lua::metamethod;

		// glm::mat4 //
		auto clss = root.beginClass<glm::mat4>("mat4");
		clss.addStaticFunction(meta::call, &constructor)
			.addFunction(meta::eq, &eq)
			.addFunction(meta::add, &add)
			.addFunction(meta::sub, &sub)
			.addFunction(meta::mul, &mul)
			.addFunction(meta::div, &div)
			.addFunction(meta::unm, &neg)
			.addFunction(meta::len, &len)
			.addFunction(meta::tostring, &toString)
			.addFunction(meta::index, &index)
			.addFunction(meta::newindex, &newindex)
			.addFunction("transpose", &transpose)
			.addFunction("inverse", &inverse)
			.addFunction("translate", &translate)
			.addFunction("scale", &scale)
			.addFunction("rotate", &rotate)
			.addStaticFunction("identity", &s_identity)
			.addStaticFunction("lookAt", &s_lookAt)
			.addStaticFunction("ortho", &s_ortho)
			.addStaticFunction("perspective", &s_perspective)
			;
		root = clss.endClass();

		return true;
	}




	namespace LUA_static
	{
		static float radians(float degrees) {  }
	}
	
	static defineLuaLibraryConstructor(LUA_glmLib, root, state)
	{
		if (!LUA_glmLib_vec2(root, state))
			return false;

		if (!LUA_glmLib_vec3(root, state))
			return false;

		if (!LUA_glmLib_vec4(root, state))
			return false;

		if (!LUA_glmLib_mat4(root, state))
			return false;

		root.addFunction("radians", &glm::radians<float>);
		root.addFunction("degrees", &glm::degrees<float>);

		return true;
	}
}
