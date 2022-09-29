#include "image.h"

#include <filesystem>
#include <algorithm>

#include "utils.h"
#include "io_utils.h"
#include "logger.h"

#include "bmp_decoder.h"
#include "jpeg_decoder.h"
#include "png_decoder.h"


namespace fs = std::filesystem;
using Path = fs::path;



void Image::clear()
{
	_width = 0;
	_height = 0;
	_bit_depth = BitDepth::unsupported;
	_invertedColor = false;
	_invertedRows = false;
	_data.clear();
}

bool Image::load(std::string_view filename)
{
	Path path = Path(filename);
	auto ext = utils::lower(path.extension().string());

	if (ext == ".png")
		return loadFromPNG(filename);
	if (ext == ".jpg" || ext == ".jpeg")
		return loadFromJPG(filename);
	if (ext == ".bmp")
		return loadFromBMP(filename);

	clear();

	logger::error("Unsupported image file format: {}. Filepath: {}.", ext, filename);
	return false;
}

bool Image::loadFromBMP(std::string_view filename)
{
	clear();

	BMP::File file;
	if (!BMP::read(file, filename))
		return false;

	if (file.bmp_info_header.bit_count != 32 && file.bmp_info_header.bit_count != 24)
	{
		logger::error("Unsupported BMP image bit-depth: {}. Filepath: {}.", file.bmp_info_header.bit_count, filename);
		return false;
	}

	_width = file.bmp_info_header.width;
	_height = file.bmp_info_header.height;
	_bit_depth = file.bmp_info_header.bit_count == 32 ? BitDepth::bd32 : BitDepth::bd24;
	_invertedColor = true;
	_data = std::move(file.data);

	return true;
}

bool Image::loadFromJPG(std::string_view filename)
{
	clear();

	try
	{
		marengo::jpeg::Image file{ std::string(filename) };

		auto psize = file.getPixelSize();
		if (psize != 3 && psize != 4)
		{
			logger::error("Error during JPEG image file read: Not supported images with {} bit-depth.", (psize * 8));
			return false;
		}
		
		_width = std::uint32_t(file.getWidth());
		_height = std::uint32_t(file.getHeight());
		_bit_depth = psize == 4 ? BitDepth::bd32 : BitDepth::bd24;

		const std::size_t rowsize = _width * psize;
		const std::size_t lastRowPos = std::size_t(_height - 1);

		_data.resize(file.getPixelsCount() * psize);
		for (size_t row = 0; row < _height; ++row)
			for (size_t col = 0; col < _width; ++col)
			{
				const std::size_t idx = ((lastRowPos - row) * rowsize) + (col * psize);
				auto pixel = file.getPixel(col, row);
				std::memcpy(&_data[idx], pixel.data(), psize);
			}


		return true;
	}
	catch (const std::runtime_error& ex)
	{
		logger::error("Error during JPEG image file read: {}.", ex.what());
		return false;
	}
}

bool Image::loadFromPNG(std::string_view filename)
{
	clear();

	std::vector<std::uint8_t> png;

	unsigned int width, height;
	std::vector<std::uint8_t> data;
	
	lodepng::State state;
	state.info_raw.colortype = LodePNGColorType::LCT_RGBA;
	state.info_raw.bitdepth = 8;

	auto result = lodepng::load_file(png, std::string(filename));
	if (result != 0)
	{
		logger::error("Error during PNG image file read: {}.", lodepng_error_text(result));
		return false;
	}

	result = lodepng::decode(data, width, height, state, png);
	if (result != 0)
	{
		logger::error("Error during PNG image file read: {}.", lodepng_error_text(result));
		return false;
	}

	_width = width;
	_height = height;
	_bit_depth = BitDepth::bd32;
	invertRows(_data, data);

	return true;
}





void Image::invertRows()
{
	std::vector<std::uint8_t> temp;
	
	invertRows(temp, _data);

	_data = std::move(temp);
	_invertedRows = !_invertedRows;
}

void Image::invertRows(std::vector<std::uint8_t>& dst, const std::vector<std::uint8_t>& src)
{
	const std::size_t rowsize = _width * pixelsize();
	const std::size_t lastRowPos = std::size_t(_height - 1);

	dst.resize(src.size());

	for (std::int64_t row = lastRowPos; row >= 0; --row)
	{
		std::memcpy(&dst[(lastRowPos - row) * rowsize], &src[row * rowsize], rowsize);
	}
}
