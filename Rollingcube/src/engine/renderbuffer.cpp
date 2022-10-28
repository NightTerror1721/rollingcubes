#include "renderbuffer.h"

#include "utils/logger.h"


bool RenderBuffer::create(InternalFormat internalFormat, SizeType width, SizeType height)
{
	if (isCreated())
	{
		logger::error("Attempt to create an already created RenderBuffer");
		return false;
	}

	glGenRenderbuffers(1, &_id);
	if (_id == 0)
	{
		logger::error("Unable to create renderbuffer with internal format {} and dimensions [{}, {}]!",
			GLenum(internalFormat), width, height);
		return false;
	}

	bind();
	glRenderbufferStorage(GL_RENDERBUFFER, GLenum(internalFormat), width, height);

	_internalFormat = internalFormat;
	_width = width;
	_height = height;

	return true;
}

bool RenderBuffer::resize(SizeType width, SizeType height)
{
	if (!isCreated())
		return false;

	const InternalFormat internalFormat = _internalFormat;
	destroy();
	return create(internalFormat, width, height);
}

void RenderBuffer::destroy()
{
	if (isCreated())
		glDeleteRenderbuffers(1, &_id);

	_id = 0;
	_internalFormat = InternalFormat(0);
	_width = 0;
	_height = 0;
	_depthBits = -1;
	_stencilBits = -1;
}

GLint RenderBuffer::getDepthBits() const
{
	if (_depthBits == -1 && isCreated())
	{
		bind();
		glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_DEPTH_SIZE, &_depthBits);
	}
	return _depthBits;
}

GLint RenderBuffer::getStencilBits() const
{
	if (_stencilBits == -1 && isCreated())
	{
		bind();
		glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_STENCIL_SIZE, &_stencilBits);
	}
	return _stencilBits;
}
