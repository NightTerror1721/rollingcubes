#include "texture.h"

#include "utils/logger.h"
#include "utils/exception_utils.h"


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

bool CubeMapTexture::loadFromJson(std::string_view path, std::string_view directoryPath)
{
	try
	{
		JsonValue value = json::read(path);
		return loadFromJson(value, directoryPath);
	}
	catch (const std::exception& ex)
	{
		logger::error("Error during cubemap texture json file load: {}.", ex.what());
		return false;
	}
}

bool CubeMapTexture::loadFromJson(const JsonValue& json, std::string_view directoryPath)
{
	if (!json.is_object())
	{
		logger::error("Ill-formed cubemap texture json file.");
		return false;
	}

	try
	{
		const auto directory = Path(directoryPath);
		return loadFromImage({
			extractPathFromJson(json, "front", directory),
			extractPathFromJson(json, "back", directory),
			extractPathFromJson(json, "left", directory),
			extractPathFromJson(json, "right", directory),
			extractPathFromJson(json, "top", directory),
			extractPathFromJson(json, "bottom", directory)
		});
	}
	catch (const std::exception& ex)
	{
		logger::error("Error during cubemap texture json file load: {}.", ex.what());
		return false;
	}
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

std::string CubeMapTexture::extractPathFromJson(const JsonValue& json, const std::string& filename, const Path& directory)
{
	if (!json.contains(filename))
		throw utils::formatException("Cubemap '{}' face not found.", filename);

	auto jpath = json.at(filename);
	if(!jpath.is_string())
		throw utils::formatException("Cubemap '{}' face has not valid string path.", filename);

	const auto path = jpath.get_ref<const std::string&>();
	return resources::absolute(directory.empty() ? Path(path) : (directory / path)).string();
}




CubeMapTextureManager CubeMapTextureManager::Root = CubeMapTextureManager(nullptr);
