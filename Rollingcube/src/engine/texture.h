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









struct CubeMapTextureFacesFiles
{
	static constexpr std::size_t FacesCount = 6;

	static constexpr std::size_t FrontFaceIndex = 4;
	static constexpr std::size_t BackFaceIndex = 5;
	static constexpr std::size_t LeftFaceIndex = 1;
	static constexpr std::size_t RightFaceIndex = 0;
	static constexpr std::size_t TopFaceIndex = 2;
	static constexpr std::size_t BottomFaceIndex = 3;

	std::string files[FacesCount] = {};

	CubeMapTextureFacesFiles() = default;
	CubeMapTextureFacesFiles(const CubeMapTextureFacesFiles& right) : CubeMapTextureFacesFiles()
	{
		for (std::size_t i = 0; i < FacesCount; ++i)
			files[i] = right.files[i];
	}
	CubeMapTextureFacesFiles(CubeMapTextureFacesFiles&& right) noexcept : CubeMapTextureFacesFiles()
	{
		for (std::size_t i = 0; i < FacesCount; ++i)
			files[i] = std::move(right.files[i]);
	}
	~CubeMapTextureFacesFiles() = default;

	CubeMapTextureFacesFiles& operator= (const CubeMapTextureFacesFiles& right)
	{
		std::destroy_at(this);
		return *std::construct_at(this, right);
	}
	CubeMapTextureFacesFiles& operator= (CubeMapTextureFacesFiles&& right) noexcept
	{
		std::destroy_at(this);
		return *std::construct_at< CubeMapTextureFacesFiles, CubeMapTextureFacesFiles&&>(this, std::move(right));
	}

	CubeMapTextureFacesFiles(
		std::string_view frontFilename,
		std::string_view backFilename,
		std::string_view leftFilename,
		std::string_view rightFilename,
		std::string_view topFilename,
		std::string_view bottomFilename
	) : CubeMapTextureFacesFiles()
	{
		files[FrontFaceIndex] = std::string(frontFilename);
		files[BackFaceIndex] = std::string(backFilename);
		files[LeftFaceIndex] = std::string(leftFilename);
		files[RightFaceIndex] = std::string(rightFilename);
		files[TopFaceIndex] = std::string(topFilename);
		files[BottomFaceIndex] = std::string(bottomFilename);
	}

	constexpr std::string& front() { return files[FrontFaceIndex]; }
	constexpr const std::string& front() const { return files[FrontFaceIndex]; }

	constexpr std::string& back() { return files[BackFaceIndex]; }
	constexpr const std::string& back() const { return files[BackFaceIndex]; }

	constexpr std::string& left() { return files[LeftFaceIndex]; }
	constexpr const std::string& left() const { return files[LeftFaceIndex]; }

	constexpr std::string& right() { return files[RightFaceIndex]; }
	constexpr const std::string& right() const { return files[RightFaceIndex]; }

	constexpr std::string& top() { return files[TopFaceIndex]; }
	constexpr const std::string& top() const { return files[TopFaceIndex]; }

	constexpr std::string& bottom() { return files[BottomFaceIndex]; }
	constexpr const std::string& bottom() const { return files[BottomFaceIndex]; }

	constexpr std::string& operator[] (std::size_t idx) { return files[idx]; }
	constexpr const std::string& operator[] (std::size_t idx) const { return files[idx]; }
};

class CubeMapTexture
{
public:
	using Id = GLuint;
	using Format = TextureFormat;
	using FacesFiles = CubeMapTextureFacesFiles;
	using MagnificationFilter = gl::MagnificationFilter;
	using MinificationFilter = gl::MinificationFilter;
	using SizeType = GLsizei;
	using Ref = ManagerReference<CubeMapTexture>;

	static constexpr std::size_t FacesCount = FacesFiles::FacesCount;

	static constexpr std::size_t FrontFaceIndex = FacesFiles::FrontFaceIndex;
	static constexpr std::size_t BackFaceIndex = FacesFiles::BackFaceIndex;
	static constexpr std::size_t LeftFaceIndex = FacesFiles::LeftFaceIndex;
	static constexpr std::size_t RightFaceIndex = FacesFiles::RightFaceIndex;
	static constexpr std::size_t TopFaceIndex = FacesFiles::TopFaceIndex;
	static constexpr std::size_t BottomFaceIndex = FacesFiles::BottomFaceIndex;

private:
	Id _id = 0;
	SizeType _width = 0;
	SizeType _height = 0;
	Format _format = Format(0);
	FacesFiles _files;

public:
	CubeMapTexture() = default;
	CubeMapTexture(const CubeMapTexture&) = delete;
	CubeMapTexture& operator= (const CubeMapTexture&) = delete;

	inline CubeMapTexture(CubeMapTexture&& right) noexcept :
		_id(right._id), _width(right._width), _height(right._height), _format(right._format), _files(std::move(right._files))
	{
		right._id = 0;
		right._width = 0;
		right._height = 0;
		right._format = Format(0);
	}

	CubeMapTexture& operator= (CubeMapTexture&& right) noexcept
	{
		std::destroy_at(this);
		return *std::construct_at<CubeMapTexture, CubeMapTexture&&>(this, std::move(right));
	}

	inline ~CubeMapTexture() { destroy(); }

public:
	constexpr bool isCreated() const { return _id != 0; }
	constexpr Id getId() const { return _id; }
	constexpr SizeType getWidth() const { return _width; }
	constexpr SizeType getHeight() const { return _height; }
	constexpr bool hasFile(std::size_t faceIdx) const { return !_files[faceIdx].empty(); }
	constexpr std::string_view getFilePath(std::size_t faceIdx) const { return _files[faceIdx]; }

	inline void bind() { glBindTexture(GL_TEXTURE_CUBE_MAP, _id); }
	inline void unbind() { glBindTexture(GL_TEXTURE_CUBE_MAP, 0); }

	inline void activate(GLint textureUnit = 0) { if (checkIsCreated()) glActiveTexture(GL_TEXTURE0 + textureUnit), bind(); }

	inline void setFilter(MagnificationFilter filter) { if (checkIsCreated()) glTextureParameteri(_id, GL_TEXTURE_MAG_FILTER, GLint(filter)); }
	inline void setFilter(MinificationFilter filter) { if (checkIsCreated()) glTextureParameteri(_id, GL_TEXTURE_MIN_FILTER, GLint(filter)); }

	inline void setRepeat(bool repeat)
	{
		if (checkIsCreated())
		{
			GLint param = repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE;
			glTextureParameteri(_id, GL_TEXTURE_WRAP_S, param);
			glTextureParameteri(_id, GL_TEXTURE_WRAP_T, param);
			glTextureParameteri(_id, GL_TEXTURE_WRAP_R, param);
		}
	}

	bool loadFromImage(const FacesFiles& filenames);

	void destroy();

public:
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



class CubeMapTextureManager : public Manager<CubeMapTexture>
{
private:
	static CubeMapTextureManager Root;

public:
	static inline CubeMapTextureManager& root() { return Root; }

	inline CubeMapTextureManager createChildManager() { return CubeMapTextureManager(this); }

	inline Reference create(const IdType& id) { return emplace(id); }

	inline Reference loadFromImage(const IdType& id, const CubeMapTexture::FacesFiles& filenames)
	{
		Reference ref = create(id);
		if (!ref)
			return nullptr;

		if (!ref->loadFromImage(filenames))
			return destroy(id), nullptr;

		return ref;
	}

	inline Reference loadFromImage(const IdType& id,
		std::string_view frontFilename,
		std::string_view backFilename,
		std::string_view leftFilename,
		std::string_view rightFilename,
		std::string_view topFilename,
		std::string_view bottomFilename
	) {
		return loadFromImage(id, {
			frontFilename,
			backFilename,
			leftFilename,
			rightFilename,
			topFilename,
			bottomFilename
		});
	}

private:
	inline explicit CubeMapTextureManager(Manager<CubeMapTexture>* parent) :
		Manager(parent)
	{}
};
