#pragma once

#include "core/gl.h"
#include "utils/image.h"


enum class TextureComponentType : GLint
{
	depth_component = GL_DEPTH_COMPONENT,
	depth_stencil = GL_DEPTH_STENCIL,
	red = GL_RED,
	rg = GL_RG,
	rgb = GL_RGB,
	rgba = GL_RGBA
};

enum class TextureFormat : GLenum
{
	color_index = GL_COLOR_INDEX,
	red = GL_RED,
	green = GL_GREEN,
	blue = GL_BLUE,
	alpha = GL_ALPHA,
	rgb = GL_RGB,
	rgba = GL_RGBA,
	bgr = GL_BGR,
	bgra = GL_BGRA,
	luminance = GL_LUMINANCE,
	luminance_alpha = GL_LUMINANCE_ALPHA
};

enum class TextureDataType : GLenum
{
	unsigned_byte = GL_UNSIGNED_BYTE,
	byte = GL_BYTE,
	bitmap = GL_BITMAP,
	unsigned_short = GL_UNSIGNED_SHORT,
	short_ = GL_SHORT,
	unsigned_int = GL_UNSIGNED_INT,
	int_ = GL_INT,
	float_ = GL_FLOAT
};

struct TextureGLInfo
{
	GLint level = 0;
	TextureComponentType internal_format = TextureComponentType::rgb;
	GLsizei width = 0;
	GLsizei height = 0;
	TextureFormat format = TextureFormat::rgb;
	TextureDataType type = TextureDataType::unsigned_byte;
	bool generate_mipmap = true;
};


class Texture
{
public:
	static constexpr GLuint invalid_id = 0;

private:
	GLuint _id = invalid_id;

public:
	Texture() = default;
	Texture(const Texture&) = delete;
	inline Texture(Texture&& t) noexcept : _id(t._id) { t._id = invalid_id; }

	Texture& operator= (const Texture&) = delete;

	bool operator== (const Texture&) const = default;


	Texture& operator= (Texture&& t) noexcept;
	~Texture();

	void clear();

	bool createTexture(const TextureGLInfo& info, const void* pixels);

	bool load(std::string_view filename);


	inline void bind() const { glBindTexture(GL_TEXTURE_2D, _id); }
	inline void unbind() const { glBindTexture(GL_TEXTURE_2D, invalid_id); }


	template <typename _Ty> void setParameter(GLenum parameterId, _Ty value, GLenum target = GL_TEXTURE_2D) const;

	template <typename _Ty> void setParameterArray(GLenum parameterId, const _Ty* value, GLenum target = GL_TEXTURE_2D) const;

	template <typename _Ty> void setParameterIArray(GLenum parameterId, const _Ty* values, GLenum target = GL_TEXTURE_2D) const;
};


template <> inline void Texture::setParameter<GLfloat>(GLenum parameterId, GLfloat value, GLenum target) const { glTexParameterf(target, parameterId, value); }
template <> inline void Texture::setParameter<GLint>(GLenum parameterId, GLint value, GLenum target) const { glTexParameteri(target, parameterId, value); }

template <> inline void Texture::setParameterArray<GLfloat>(GLenum parameterId, const GLfloat* values, GLenum target) const
{
	glTexParameterfv(target, parameterId, values);
}
template <> inline void Texture::setParameterArray<GLint>(GLenum parameterId, const GLint* values, GLenum target) const
{
	glTexParameteriv(target, parameterId, values);
}

template <> inline void Texture::setParameterIArray<GLint>(GLenum parameterId, const GLint* values, GLenum target) const
{
	glTexParameterIiv(target, parameterId, values);
}

template <> inline void Texture::setParameterIArray<GLuint>(GLenum parameterId, const GLuint* values, GLenum target) const
{
	glTexParameterIuiv(target, parameterId, values);
}
