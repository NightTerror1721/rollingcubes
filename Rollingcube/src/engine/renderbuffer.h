#pragma once

#include "core/gl.h"


enum class RenderBufferInternalFormat : GLenum
{
	RGB8 = GL_RGB8,
	RGB8I = GL_RGB8I,
	RGB8UI = GL_RGB8UI,
	RGB16 = GL_RGB16,
	RGB16F = GL_RGB16F,
	RGB16I = GL_RGB16I,
	RGB16UI = GL_RGB16UI,
	RGB32F = GL_RGB32F,
	RGB32I = GL_RGB32I,
	RGB32UI = GL_RGB32UI,

	RGBA8 = GL_RGBA8,
	RGBA8I = GL_RGBA8I,
	RGBA8UI = GL_RGBA8UI,
	RGBA16 = GL_RGBA16,
	RGBA16F = GL_RGBA16F,
	RGBA16I = GL_RGBA16I,
	RGBA16UI = GL_RGBA16UI,
	RGBA32F = GL_RGBA32F,
	RGBA32I = GL_RGBA32I,
	RGBA32UI = GL_RGBA32UI,

	DepthComponent16 = GL_DEPTH_COMPONENT16,
	DepthComponent24 = GL_DEPTH_COMPONENT24,
	DepthComponent32 = GL_DEPTH_COMPONENT32,
	DepthComponent32F = GL_DEPTH_COMPONENT32F,

	Depth24Stencil8 = GL_DEPTH24_STENCIL8,
	Depth32FStencil8 = GL_DEPTH32F_STENCIL8,
};


class RenderBuffer
{
public:
	using Id = GLuint;
	using InternalFormat = RenderBufferInternalFormat;
	using SizeType = GLsizei;

private:
	Id _id = 0;
	InternalFormat _internalFormat = InternalFormat(0);
	SizeType _width = 0;
	SizeType _height = 0;
	mutable GLint _depthBits = -1;
	mutable GLint _stencilBits = -1;

public:
	constexpr RenderBuffer() = default;
	RenderBuffer(const RenderBuffer&) = delete;
	constexpr RenderBuffer(RenderBuffer&&) noexcept = default;
	inline ~RenderBuffer() { destroy(); }

	RenderBuffer& operator= (const RenderBuffer&) = delete;
	RenderBuffer& operator= (RenderBuffer&&) noexcept = default;

public:
	constexpr bool isCreated() const { return _id != 0; }

	constexpr Id getId() const { return _id; }
	constexpr InternalFormat getInternalFormat() const { return _internalFormat; }
	constexpr SizeType getWidth() const { return _width; }
	constexpr SizeType getHeight() const { return _height; }

	inline void bind() const { glBindRenderbuffer(GL_RENDERBUFFER, _id); }

public:
	bool create(InternalFormat internalFormat, SizeType width, SizeType height);

	bool resize(SizeType width, SizeType height);

	void destroy();

	GLint getDepthBits() const;
	GLint getStencilBits() const;
};
