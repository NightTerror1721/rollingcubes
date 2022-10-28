#pragma once

#include <memory>
#include <vector>

#include "core/gl.h"
#include "renderbuffer.h"
#include "texture.h"


class FrameBuffer
{
public:
	using Id = GLuint;
	using InternalFormat = RenderBuffer::InternalFormat;
	using SizeType = GLsizei;

private:
	Id _id = 0;
	std::unique_ptr<RenderBuffer> _colorRenderBuffer = nullptr;
	std::unique_ptr<RenderBuffer> _depthRenderBuffer = nullptr;
	std::unique_ptr<Texture> _texture = nullptr;

	SizeType _width = 0;
	SizeType _height = 0;
	mutable GLint _depthBits = -1;
	mutable GLint _stencilBits = -1;

public:
	FrameBuffer() = default;
	FrameBuffer(const FrameBuffer&) = delete;
	FrameBuffer(FrameBuffer&&) noexcept = default;
	inline ~FrameBuffer() { destroy(); }

	FrameBuffer& operator= (const FrameBuffer&) = delete;
	FrameBuffer& operator= (FrameBuffer&&) noexcept = default;

public:
	constexpr bool isCreated() const { return _id != 0; }

	constexpr Id getId() const { return _id; }
	constexpr SizeType getWidth() const { return _width; }
	constexpr SizeType getHeight() const { return _height; }

	constexpr Texture::Ref getTexture() const { return _texture.get(); }

	inline void bind() const { glBindFramebuffer(GL_FRAMEBUFFER, _id); }
	inline void bindAsRead() const { glBindFramebuffer(GL_READ_FRAMEBUFFER, _id); }
	inline void bindAsDraw() const { glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _id); }

	inline void setFullViewport() const { glViewport(0, 0, _width, _height); }

public:
	bool createWithColorAndDepthWithDefaultScreenSize();
	bool createWithColorAndDepth(SizeType width, SizeType height);

	bool resize(SizeType width, SizeType height);

	void destroy();

	void copyColorToDefaultFrameBuffer() const;

	void copyDepthFromDefaultFrameBuffer() const;

	GLint getDepthBits() const;
	GLint getStencilBits() const;

private:
	bool create(SizeType width, SizeType height, bool doBind);
	bool withColorAttachment(InternalFormat internalFormat);
	bool withDepthAttachment(InternalFormat internalFormat);
	bool withTextureColorAttachment(Texture::Format textureFormat);
	bool finishInitialization() const;

	void destroyOnlyFrameBuffer();

public:
	static std::vector<GLubyte> readColorValue(int x, int y);

public:
	struct Default
	{
		Default() = delete;
		Default(const Default&) = delete;
		Default(Default&&) noexcept = delete;
		~Default() = delete;

		Default& operator= (const Default&) = delete;
		Default& operator= (Default&&) noexcept = delete;

	public:
		static inline void bind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
		static inline void bindAsRead() { glBindFramebuffer(GL_READ_FRAMEBUFFER, 0); }
		static inline void bindAsDraw() { glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); }

	public:
		static void setFullViewport();

		static GLint getDepthBits();
		static GLint getStencilBits();
	};

public:
	class Builder
	{
	private:
		std::unique_ptr<FrameBuffer> _frameBuffer = nullptr;

	public:
		Builder() = default;
		Builder(const Builder&) = delete;
		Builder(Builder&&) noexcept = default;
		~Builder() = default;

		Builder& operator= (const Builder&) = delete;
		Builder& operator= (Builder&&) noexcept = default;

	public:
		Builder& create(SizeType width, SizeType height, bool bind = true);

		Builder& withColorAttachment(InternalFormat internalFormat = InternalFormat::RGBA8);

		Builder& withDepthAttachment(InternalFormat internalFormat = InternalFormat::DepthComponent24);

		Builder& withTextureColorAttachment(Texture::Format textureFormat = Texture::Format::rgb);

		std::unique_ptr<FrameBuffer> finishAndGetUnique();
		std::shared_ptr<FrameBuffer> finishAndGetShader();
		bool finish(FrameBuffer* fbo);
	};
};
