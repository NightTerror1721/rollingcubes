#include "texture.h"

#include "utils/logger.h"


bool Texture::createFromData(const unsigned char* data, SizeType width, SizeType height, Format format, bool generateMipmaps)
{
	if (isCreated())
		return false;

	_width = width;
	_height = height;
	_format = format;

	glGenTextures(1, &_id);
	bind();
	glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(_format), _width, _height, 0, static_cast<GLenum>(_format), GL_UNSIGNED_BYTE, data);

	if (generateMipmaps)
		glGenerateMipmap(GL_TEXTURE_2D);

	return true;
}

bool Texture::loadFromImage(std::string_view name, bool generateMipmaps)
{
	if (isCreated())
		return false;

	Image img;
	if (!img.load(name))
	{
		logger::error("Cannot load texture because it's image cannot be read.");
		return false;
	}

	if (img.hasInvertedPixelRows())
		img.invertRows();

	Format fmt = img.hasAlpha() ? Format::rgba : Format::rgb;

	if (!createFromData(img.data(), SizeType(img.width()), SizeType(img.height()), fmt, generateMipmaps))
		return false;

	_file = name;

	return true;
}

bool Texture::resize(SizeType width, SizeType height, bool generateMipmaps)
{
	if (!checkIsCreated())
		return false;

	const Format fmt = _format;
	destroy();

	return create(width, height, fmt, generateMipmaps);
}

void Texture::destroy()
{
	if (isCreated())
		glDeleteTextures(1, &_id);

	_id = 0;
	_width = 0;
	_height = 0;
	_format = Format(0);
}

GLint Texture::getNumTextureImageUnits()
{
	static GLint max_texture_units = 0;
	if (max_texture_units == 0)
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_texture_units);

	return max_texture_units;
}




TextureManager TextureManager::Root = TextureManager(nullptr);







bool CubeMapTexture::loadFromImage(const FacesFiles& filenames)
{
	if (isCreated())
		return false;

	glGenTextures(1, &_id);
	bind();

	for (std::size_t i = 0; i < FacesCount; i++)
	{
		const std::string& filename = filenames[i];
		if (!filename.empty())
		{
			Image img;
			if (!img.load(filename))
			{
				logger::error("Cannot load cubemap face texture because it's image cannot be read. Image filanem: {}", filename);
				continue;
			}

			if (img.hasInvertedPixelRows())
				img.invertRows();

			Format fmt = img.hasAlpha() ? Format::rgba : Format::rgb;

			glTexImage2D(
				static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i),
				0,
				static_cast<GLint>(fmt),
				static_cast<GLsizei>(img.width()),
				static_cast<GLsizei>(img.height()),
				0,
				static_cast<GLenum>(fmt),
				GL_UNSIGNED_BYTE,
				img.data()
			);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return true;
}

void CubeMapTexture::destroy()
{
	if (isCreated())
		glDeleteTextures(1, &_id);

	_id = 0;
	_width = 0;
	_height = 0;
	_format = Format(0);
	_files = {};
}




CubeMapTextureManager CubeMapTextureManager::Root = CubeMapTextureManager(nullptr);
