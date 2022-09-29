#pragma once

#include <cstdint>
#include <vector>
#include <string>


enum class ImageBitDepth
{
	unsupported,
	bd24 = 24u,
	bd32 = 32u
};

class Image
{
public:
	using BitDepth = ImageBitDepth;

private:
	std::uint32_t _width = 0;
	std::uint32_t _height = 0;
	BitDepth _bit_depth = BitDepth::unsupported;
	bool _invertedColor = false;
	bool _invertedRows = false;
	std::vector<std::uint8_t> _data;

public:
	Image() = default;
	Image(const Image&) = default;
	Image(Image&&) noexcept = default;
	~Image() = default;

	Image& operator= (const Image&) = default;
	Image& operator= (Image&&) noexcept = default;


	void clear();

	bool load(std::string_view filename);

	bool loadFromBMP(std::string_view filename);
	bool loadFromJPG(std::string_view filename);
	bool loadFromPNG(std::string_view filename);

	void invertRows();


	inline bool isValid() const { return _bit_depth == BitDepth::bd24 || _bit_depth == BitDepth::bd32; }

	inline bool hasAlpha() const { return _bit_depth == BitDepth::bd32; }

	inline bool hasInvertedColorComponents() const { return _invertedColor; }

	inline bool hasInvertedPixelRows() const { return _invertedRows; }

	inline std::uint32_t width() const { return _width; };
	inline std::uint32_t height() const { return _height; }

	inline std::size_t datasize() const { return _data.size(); }
	inline const std::uint8_t* data() const { return _data.data(); }

	inline std::size_t pixelsize() const { return _bit_depth == BitDepth::bd24 ? 3 : 4; }


private:
	void invertRows(std::vector<std::uint8_t>& dst, const std::vector<std::uint8_t>& src);
};
