#pragma once

#pragma warning(push)
#pragma warning(disable: 26819)

//#include <SDL.h>
//#include <GL/glew.h>
//#include <SDL_opengl.h>
//#include <GL/GLU.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#pragma warning(pop)

#include <unordered_map>
#include <vector>
#include <memory>

#include "utils/logger.h"


namespace gl
{
	bool initGLFW();
	bool initGLEW();
	void terminate();
}


namespace gl
{
	enum class DataType : GLenum
	{
		Byte = GL_BYTE,
		UnsignedByte = GL_UNSIGNED_BYTE,
		Short = GL_SHORT,
		UnsignedShort = GL_UNSIGNED_SHORT,
		Int = GL_INT,
		UnsignedInt = GL_UNSIGNED_INT,

		HalfFloat = GL_HALF_FLOAT,
		Float = GL_FLOAT,
		Double = GL_DOUBLE,
		Fixed = GL_FIXED,
		Int_2_10_10_10_rev = GL_INT_2_10_10_10_REV,
		UnsignedInt_2_10_10_10_rev = GL_UNSIGNED_INT_2_10_10_10_REV,
		UnsignedInt_10f_11f_11f_rev = GL_UNSIGNED_INT_10F_11F_11F_REV
	};

	enum class MagnificationFilter : GLint
	{
		Nearest = GL_NEAREST,
		Bilinear = GL_LINEAR
	};

	enum class MinificationFilter : GLint
	{
		Nearest = GL_NEAREST,
		Bilinear = GL_LINEAR,
		NearestMipmap = GL_NEAREST_MIPMAP_NEAREST,
		BilinearMipmap = GL_LINEAR_MIPMAP_LINEAR,
		Trilinear = GL_LINEAR_MIPMAP_LINEAR
	};

	enum class DepthFunction : GLenum
	{
		Never = GL_NEVER,
		Less = GL_LESS,
		Equal = GL_EQUAL,
		Greater = GL_GREATER,
		NotEqual = GL_NOTEQUAL,
		GreaterOrEqual = GL_GEQUAL,
		Always = GL_ALWAYS,

		Default = Less
	};

	enum class CullFace : GLenum
	{
		Front = GL_FRONT,
		Back = GL_BACK,
		FrontAndBack = GL_FRONT_AND_BACK,

		Default = Back
	};

	enum class FrontFace : GLenum
	{
		Clockwise = GL_CW,
		CounterClockwise = GL_CCW,

		Default = CounterClockwise
	};
}

namespace gl
{
	inline void setDepthFunction(DepthFunction fn) { glDepthFunc(GLenum(fn)); }
	inline void enableDepthTest() { glEnable(GL_DEPTH_TEST); }
	inline void disableDepthTest() { glDisable(GL_DEPTH_TEST); }

	inline void setCullFace(CullFace face) { glCullFace(GLenum(face)); }
	inline void setFrontFace(FrontFace face) { glFrontFace(GLenum(face)); }
	inline void enableCullFace() { glEnable(GL_CULL_FACE); }
	inline void disableCullFace() { glDisable(GL_CULL_FACE); }
}
