#include "framebuffer.h"

#include "core/window.h"
#include "utils/logger.h"


bool FrameBuffer::createWithColorAndDepthWithDefaultScreenSize()
{
	window::Dimension winSize = window::getMainWindowSize();
	return createWithColorAndDepth(winSize.width, winSize.height);
}

bool FrameBuffer::createWithColorAndDepth(SizeType width, SizeType height)
{
	if (isCreated())
	{
		logger::error("Attempt to create an already created FrameBuffer");
		return false;
	}

	glGenFramebuffers(1, &_id);
	if (_id == 0)
	{
		logger::error("Unable to create framebuffer!");
		return false;
	}
	bind();

	// Create color render buffer and attach it to FBO //
	auto colorRenderBuffer = std::make_unique<RenderBuffer>();
	if(!colorRenderBuffer->create(InternalFormat::RGBA8, width, height))
	{
		logger::error("Unable to create color attachment for the framebuffer #{}!", _id);
		destroy();
		return false;
	}
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderBuffer->getId());

	// Create depth buffer and attach it to FBO //
	auto depthRenderBuffer = std::make_unique<RenderBuffer>();
	if (!depthRenderBuffer->create(InternalFormat::DepthComponent24, width, height))
	{
		logger::error("Unable to create depth attachment for the framebuffer #{}!", _id);
		destroy();
		return false;
	}
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer->getId());

	// Check FBO status when all attachments have been attached //
	const auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		destroy();
		return false;
	}

	_colorRenderBuffer = std::move(colorRenderBuffer);
	_depthRenderBuffer = std::move(depthRenderBuffer);
	_width = width;
	_height = height;

	return true;
}

bool FrameBuffer::resize(SizeType newWidth, SizeType newHeight)
{
	if (!isCreated())
		return false;

	destroyOnlyFrameBuffer();

	glGenFramebuffers(1, &_id);
	if (_id == 0)
	{
		logger::error("Unable to create framebuffer during resizing!");
		destroy();
		return false;
	}
	bind();

	if (_colorRenderBuffer != nullptr)
	{
		if (!_colorRenderBuffer->resize(newWidth, newHeight))
		{
			logger::error("Unable to resize color attachment for the framebuffer #{}!", _id);
			destroy();
			return false;
		}

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _colorRenderBuffer->getId());
	}

	if (_depthRenderBuffer != nullptr)
	{
		if (!_depthRenderBuffer->resize(newWidth, newHeight))
		{
			logger::error("Unable to resize depth attachment for the framebuffer #{}!", _id);
			destroy();
			return false;
		}

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthRenderBuffer->getId());
	}

	if (_texture != nullptr)
	{
		if (!_texture->resize(newWidth, newHeight))
		{
			logger::error("Unable to resize texture for the framebuffer #{}!", _id);
			destroy();
			return false;
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture->getId(), 0);
	}

	const auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		destroy();
		return false;
	}

	_width = newWidth;
	_height = newHeight;

	return true;
}

void FrameBuffer::destroy()
{
	_colorRenderBuffer.reset();
	_depthRenderBuffer.reset();
	_texture.reset();

	destroyOnlyFrameBuffer();
}

void FrameBuffer::copyColorToDefaultFrameBuffer() const
{
	window::Dimension winSize = window::getMainWindowSize();

	bindAsRead();
	Default::bindAsDraw();
	glBlitFramebuffer(0, 0, _width, _height, 0, 0, winSize.width, winSize.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void FrameBuffer::copyDepthFromDefaultFrameBuffer() const
{
	window::Dimension winSize = window::getMainWindowSize();

	bindAsRead();
	Default::bindAsDraw();
	glBlitFramebuffer(0, 0, winSize.width, winSize.height, 0, 0, _width, _height, GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}

GLint FrameBuffer::getDepthBits() const
{
	if (_depthBits == -1 && isCreated())
	{
		bindAsRead();

		glGetFramebufferAttachmentParameteriv(GL_READ_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE, &_depthBits);
		const auto error = glGetError();
		if (error != GL_NO_ERROR)
			logger::error("Could not read number of depth bits for framebuffer #{}: {}. Probably it has no depth attachment!", _id, error);
	}
	return _depthBits;
}

GLint FrameBuffer::getStencilBits() const
{
	if (_stencilBits == -1 && isCreated())
	{
		bindAsRead();

		glGetFramebufferAttachmentParameteriv(GL_READ_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE, &_stencilBits);
		const auto error = glGetError();
		if (error != GL_NO_ERROR)
			logger::error("Could not read number of depth bits for framebuffer #{}: {}. Probably it has no depth attachment!", _id, error);
	}
	return _stencilBits;
}

bool FrameBuffer::create(SizeType width, SizeType height, bool doBind)
{
	glGenFramebuffers(1, &_id);
	if (_id == 0)
	{
		logger::error("Unable to create framebuffer!");
		return false;
	}

	if(doBind)
		bind();

	return true;
}

bool FrameBuffer::withColorAttachment(InternalFormat internalFormat)
{
	if (!isCreated())
		return false;

	// Create color render buffer and attach it to FBO //
	auto colorRenderBuffer = std::make_unique<RenderBuffer>();
	if (!colorRenderBuffer->create(internalFormat, _width, _height))
	{
		logger::error("Unable to create color attachment for the framebuffer #{}!", _id);
		destroy();
		return false;
	}

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderBuffer->getId());
	_colorRenderBuffer = std::move(colorRenderBuffer);
	return true;
}

bool FrameBuffer::withDepthAttachment(InternalFormat internalFormat)
{
	if (!isCreated())
		return false;

	// Create depth buffer and attach it to FBO //
	auto depthRenderBuffer = std::make_unique<RenderBuffer>();
	if (!depthRenderBuffer->create(internalFormat, _width, _height))
	{
		logger::error("Unable to create depth attachment for the framebuffer #{}!", _id);
		destroy();
		return false;
	}

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer->getId());
	_depthRenderBuffer = std::move(depthRenderBuffer);
	return true;
}

bool FrameBuffer::withTextureColorAttachment(Texture::Format textureFormat)
{
	if (!isCreated())
		return false;

	// Create an empty texture with no data and same size as framebuffer has //
	_texture = std::make_unique<Texture>();
	_texture->create(_width, _height, textureFormat, false);
	_texture->bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture->getId(), 0);
	return true;
}

bool FrameBuffer::finishInitialization() const
{
	if (!isCreated())
		return false;

	const auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
		return false;

	return true;
}

void FrameBuffer::destroyOnlyFrameBuffer()
{
	if (isCreated())
		glDeleteFramebuffers(1, &_id);

	_id = 0;
	_width = 0;
	_height = 0;
	_depthBits = -1;
	_stencilBits = -1;
}

std::vector<GLubyte> FrameBuffer::readColorValue(int x, int y)
{
	std::vector<GLubyte> result(4, 0);
	glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, result.data());
	return result;
}





void FrameBuffer::Default::setFullViewport()
{
	window::Dimension winSize = window::getMainWindowSize();

	glViewport(0, 0, winSize.width, winSize.height);
}

GLint FrameBuffer::Default::getDepthBits()
{
	GLint depthBits = -1;;
	bindAsRead();
	glGetFramebufferAttachmentParameteriv(GL_READ_FRAMEBUFFER, GL_DEPTH, GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE, &depthBits);
	return depthBits;
}

GLint FrameBuffer::Default::getStencilBits()
{
	GLint stencilBits = -1;
	bindAsRead();
	glGetFramebufferAttachmentParameteriv(GL_READ_FRAMEBUFFER, GL_STENCIL, GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE, &stencilBits);
	return stencilBits;
}





FrameBuffer::Builder& FrameBuffer::Builder::create(SizeType width, SizeType height, bool bind)
{
	_frameBuffer = std::make_unique<FrameBuffer>();
	_frameBuffer->create(width, height, bind);
	return *this;
}

FrameBuffer::Builder& FrameBuffer::Builder::withColorAttachment(InternalFormat internalFormat)
{
	if (_frameBuffer != nullptr)
		_frameBuffer->withColorAttachment(internalFormat);
	return *this;
}

FrameBuffer::Builder& FrameBuffer::Builder::withDepthAttachment(InternalFormat internalFormat)
{
	if (_frameBuffer != nullptr)
		_frameBuffer->withDepthAttachment(internalFormat);
	return *this;
}

FrameBuffer::Builder& FrameBuffer::Builder::withTextureColorAttachment(Texture::Format textureFormat)
{
	if (_frameBuffer != nullptr)
		_frameBuffer->withTextureColorAttachment(textureFormat);
	return *this;
}

std::unique_ptr<FrameBuffer> FrameBuffer::Builder::finishAndGetUnique()
{
	if (_frameBuffer == nullptr || !_frameBuffer->finishInitialization())
		return nullptr;
	return std::move(_frameBuffer);
}

std::shared_ptr<FrameBuffer> FrameBuffer::Builder::finishAndGetShader()
{
	if (_frameBuffer == nullptr || !_frameBuffer->finishInitialization())
		return nullptr;
	return std::move(_frameBuffer);
}

bool FrameBuffer::Builder::finish(FrameBuffer* fbo)
{
	if (_frameBuffer == nullptr || !_frameBuffer->finishInitialization())
		return false;
	*fbo = std::move(*_frameBuffer);
	_frameBuffer.reset();
	return true;
}
