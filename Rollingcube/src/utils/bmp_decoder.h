#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <optional>


namespace BMP
{
	#pragma pack(push, 1)
	struct FileHeader
	{
		static constexpr std::uint16_t default_file_type = 0x4d42;

		std::uint16_t file_type = default_file_type;
		std::uint32_t file_size = 0;
		std::uint16_t reserved1 = 0;
		std::uint16_t reserved2 = 0;
		std::uint32_t offset_data = 0;
	};
	#pragma pack(pop)

	struct InfoHeader
	{
		std::uint32_t size = 0;
		std::int32_t width = 0;
		std::int32_t height = 0;
		std::uint16_t planes = 1;
		std::uint16_t bit_count = 0;
		std::uint32_t compression = 0;
		std::uint32_t size_image = 0;
		std::int32_t x_pixels_per_meter = 0;
		std::int32_t y_pixels_per_meter = 0;
		std::uint32_t colors_used = 0;
		std::uint32_t colors_important = 0;
	};

	struct ColorHeader
	{
		std::uint32_t red_mask = 0x00ff0000;
		std::uint32_t green_mask = 0x0000ff00;
		std::uint32_t blue_mask = 0x000000ff;
		std::uint32_t alpha_mask = 0xff000000;
		std::uint32_t color_space_type = 0x73524742;
		std::uint32_t unused[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	};

	struct File
	{
		FileHeader file_header;
		InfoHeader bmp_info_header;
		ColorHeader bmp_color_header;
		uint32_t row_stride = 0;
		std::vector<std::uint8_t> data;

		constexpr File() = default;
		constexpr File(const File&) = default;
		constexpr File(File&&) noexcept = default;
		constexpr ~File() = default;

		constexpr File& operator= (const File&) = default;
		constexpr File& operator= (File&&) noexcept = default;
	};


	bool read(File& bmp_file, std::string_view filename);
	inline File read(std::string_view filename)
	{
		File file;
		read(file, filename);
		return file;
	}

	inline std::optional<File> safe_read(std::string_view filename)
	{
		File file;
		if (!read(file, filename))
			return {};
		return file;
	}
}
