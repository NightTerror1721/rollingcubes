#include "texture.h"

#include "utils/logger.h"

Texture::~Texture()
{
	clear();
}

Texture& Texture::operator= (Texture&& t) noexcept
{
	this->~Texture();
	_id = t._id;
	t._id = invalid_id;
	return *this;
}


void Texture::clear()
{

}

bool Texture::createTexture(const TextureGLInfo& info, const void* pixels)
{
	if (_id != invalid_id)
	{
		logger::warn("Attempt to rebuild an existing texture.");
		return false;
	}

	glGenTextures(1, &_id);
	glBindTexture(GL_TEXTURE_2D, _id);
	glTexImage2D(
		GL_TEXTURE_2D,
		info.level,
		static_cast<GLint>(info.internal_format),
		info.width,
		info.height,
		0,
		static_cast<GLenum>(info.format),
		static_cast<GLenum>(info.type),
		pixels
	);
	if (info.generate_mipmap)
		glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, invalid_id);

	return true;
}

bool Texture::load(std::string_view filename)
{
	Image img;
	if (!img.load(filename))
	{
		logger::error("Cannot load texture because it's image cannot be read.");
		return false;
	}

	if (img.hasInvertedPixelRows())
		img.invertRows();

	TextureGLInfo info;
	info.width = GLsizei(img.width());
	info.height = GLsizei(img.height());
	info.internal_format = img.hasAlpha() ? TextureComponentType::rgba : TextureComponentType::rgb;
	info.level = 0;
	info.type = TextureDataType::unsigned_byte;
	info.generate_mipmap = true;

	if (img.hasInvertedColorComponents())
		info.format = img.hasAlpha() ? TextureFormat::bgra : TextureFormat::bgr;
	else
		info.format = img.hasAlpha() ? TextureFormat::rgba : TextureFormat::rgb;

	return createTexture(info, img.data());
}
