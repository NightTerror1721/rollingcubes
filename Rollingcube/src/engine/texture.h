#pragma once

#include <string>
#include <utility>

#include "core/gl.h"
#include "utils/image.h"
#include "utils/manager.h"

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
	luminance_alpha = GL_LUMINANCE_ALPHA,
	depth_component = GL_DEPTH_COMPONENT
};

class Texture
{
public:
	using Id = GLuint;
	using Format = TextureFormat;
	using MagnificationFilter = gl::MagnificationFilter;
	using MinificationFilter = gl::MinificationFilter;
	using SizeType = GLsizei;
	using Ref = ManagerReference<Texture>;

private:
	Id _id = 0;
	SizeType _width = 0;
	SizeType _height = 0;
	Format _format = Format(0);
	std::string _file = {};

public:
	Texture() = default;
	Texture(const Texture&) = delete;
	Texture& operator= (const Texture&) = delete;

	inline Texture(Texture&& right) noexcept :
		_id(right._id), _width(right._width), _height(right._height), _format(right._format), _file(std::move(_file))
	{
		right._id = 0;
		right._width = 0;
		right._height = 0;
		right._format = Format(0);
	}

	Texture& operator= (Texture&& right) noexcept
	{
		std::destroy_at(this);
		return *std::construct_at<Texture, Texture&&>(this, std::move(right));
	}

	inline ~Texture() { destroy(); }

public:
	constexpr bool isCreated() const { return _id != 0; }
	constexpr Id getId() const { return _id; }
	constexpr SizeType getWidth() const { return _width; }
	constexpr SizeType getHeight() const { return _height; }
	constexpr bool hasFile() const { return !_file.empty(); }
	constexpr std::string_view getFilePath() const { return _file; }

	inline void bind() { glBindTexture(GL_TEXTURE_2D, _id); }
	inline void unbind() { glBindTexture(GL_TEXTURE_2D, 0); }

	inline void activate(GLint textureUnit = 0) { if(checkIsCreated()) glActiveTexture(GL_TEXTURE0 + textureUnit), bind(); }

	inline void setFilter(MagnificationFilter filter) { if (checkIsCreated()) glTextureParameteri(_id, GL_TEXTURE_MAG_FILTER, GLint(filter)); }
	inline void setFilter(MinificationFilter filter) { if (checkIsCreated()) glTextureParameteri(_id, GL_TEXTURE_MIN_FILTER, GLint(filter)); }

	inline void setRepeat(bool repeat)
	{
		if (checkIsCreated())
		{
			GLint param = repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE;
			glTextureParameteri(_id, GL_TEXTURE_WRAP_S, param);
			glTextureParameteri(_id, GL_TEXTURE_WRAP_T, param);
		}
	}

	inline bool create(SizeType width, SizeType height, Format format, bool generateMipmaps = false)
	{
		return createFromData(nullptr, width, height, format, generateMipmaps);
	}

	bool createFromData(const unsigned char* data, SizeType width, SizeType height, Format format, bool generateMipmaps = false);

	bool loadFromImage(std::string_view filename, bool generateMipmaps = true);

	bool resize(SizeType width, SizeType height, bool generateMipmaps = false);

	void destroy();

public:
	static GLint getNumTextureImageUnits();

	static inline void deactivate(GLint textureUnit = 0) { glActiveTexture(GL_TEXTURE0 + textureUnit), glBindTexture(GL_TEXTURE_2D, 0); }

private:
	inline bool checkIsCreated()
	{
		if (!isCreated())
		{
			logger::error("Attempting to access non loaded texture!");
			return false;
		}
		return true;
	}
};


class TextureManager : public Manager<Texture>
{
private:
	static TextureManager Root;

public:
	static inline TextureManager& root() { return Root; }

	inline TextureManager createChildManager() { return TextureManager(this); }

	inline Reference create(const IdType& id) { return emplace(id); }

	Reference create(const IdType& id, Texture::SizeType width, Texture::SizeType height, Texture::Format format, bool generateMipmaps = false)
	{
		Reference ref = create(id);
		if (!ref)
			return nullptr;

		if (!ref->create(width, height, format, generateMipmaps))
			return destroy(id), nullptr;

		return ref;
	}

	Reference createFromData(const IdType& id, const unsigned char* data, Texture::SizeType width, Texture::SizeType height, Texture::Format format, bool generateMipmaps = false)
	{
		Reference ref = create(id);
		if (!ref)
			return nullptr;

		if (!ref->createFromData(data, width, height, format, generateMipmaps))
			return destroy(id), nullptr;

		return ref;
	}

	Reference loadFromImage(const IdType& id, std::string_view filename, bool generateMipmaps = true)
	{
		Reference ref = create(id);
		if (!ref)
			return nullptr;

		if (!ref->loadFromImage(filename, generateMipmaps))
			return destroy(id), nullptr;

		return ref;
	}

private:
	inline explicit TextureManager(Manager<Texture>* parent) :
		Manager(parent)
	{}
};
