#include "bmp_decoder.h"

#include <iostream>
#include <fstream>

#include "logger.h"

namespace BMP
{
    // Check if the pixel data is stored as BGRA and if the color space type is sRGB
    static bool check_color_header(ColorHeader& bmp_color_header)
    {
        ColorHeader expected_color_header;
        if (expected_color_header.red_mask != bmp_color_header.red_mask ||
            expected_color_header.blue_mask != bmp_color_header.blue_mask ||
            expected_color_header.green_mask != bmp_color_header.green_mask ||
            expected_color_header.alpha_mask != bmp_color_header.alpha_mask) {
            logger::error("Unexpected color mask format! The program expects the pixel data to be in the BGRA format.");
            return false;
        }
        if (expected_color_header.color_space_type != bmp_color_header.color_space_type) {
            logger::error("Unexpected color space type! The program expects sRGB values.");
            return false;
        }
        return true;
    }

    // Add 1 to the row_stride until it is divisible with align_stride
    static uint32_t make_stride_aligned(File& bmp, uint32_t align_stride)
    {
        uint32_t new_stride = bmp.row_stride;
        while (new_stride % align_stride != 0)
        {
            new_stride++;
        }
        return new_stride;
    }


    bool read(File& bmp, std::string_view fname)
	{
        std::ifstream inp{ std::string(fname), std::ios_base::binary };
        if (inp)
        {
            inp.read((char*)&bmp.file_header, sizeof(bmp.file_header));
            if (bmp.file_header.file_type != 0x4D42) {
                logger::error("Error! Unrecognized file format.");
                return false;
            }
            inp.read((char*)&bmp.bmp_info_header, sizeof(bmp.bmp_info_header));

            // The BMPColorHeader is used only for transparent images
            if (bmp.bmp_info_header.bit_count == 32) {
                // Check if the file has bit mask color information
                if (bmp.bmp_info_header.size >= (sizeof(InfoHeader) + sizeof(ColorHeader))) {
                    inp.read((char*)&bmp.bmp_color_header, sizeof(bmp.bmp_color_header));
                    // Check if the pixel data is stored as BGRA and if the color space type is sRGB
                    if (!check_color_header(bmp.bmp_color_header))
                        return false;
                }
                else {
                    logger::error("Error! The file \"{}\" does not seem to contain bit mask information. ", fname);
                    logger::error("Error! Unrecognized BMP file format.");
                    return false;
                }
            }

            // Jump to the pixel data location
            inp.seekg(bmp.file_header.offset_data, inp.beg);

            // Adjust the header fields for output.
            // Some editors will put extra info in the image file, we only save the headers and the data.
            if (bmp.bmp_info_header.bit_count == 32) {
                bmp.bmp_info_header.size = sizeof(InfoHeader) + sizeof(ColorHeader);
                bmp.file_header.offset_data = sizeof(FileHeader) + sizeof(InfoHeader) + sizeof(ColorHeader);
            }
            else {
                bmp.bmp_info_header.size = sizeof(InfoHeader);
                bmp.file_header.offset_data = sizeof(FileHeader) + sizeof(InfoHeader);
            }
            bmp.file_header.file_size = bmp.file_header.offset_data;

            if (bmp.bmp_info_header.height < 0) {
                logger::error("The program can treat only BMP images with the origin in the bottom left corner!");
                return false;
            }

            bmp.data.resize(bmp.bmp_info_header.width * bmp.bmp_info_header.height * bmp.bmp_info_header.bit_count / 8);

            // Here we check if we need to take into account row padding
            if (bmp.bmp_info_header.width % 4 == 0) {
                inp.read((char*)bmp.data.data(), bmp.data.size());
                bmp.file_header.file_size += static_cast<uint32_t>(bmp.data.size());
            }
            else {
                bmp.row_stride = bmp.bmp_info_header.width * bmp.bmp_info_header.bit_count / 8;
                uint32_t new_stride = make_stride_aligned(bmp, 4);
                std::vector<uint8_t> padding_row(new_stride - bmp.row_stride);

                for (int y = 0; y < bmp.bmp_info_header.height; ++y) {
                    inp.read((char*)(bmp.data.data() + bmp.row_stride * y), bmp.row_stride);
                    inp.read((char*)padding_row.data(), padding_row.size());
                }
                bmp.file_header.file_size += static_cast<uint32_t>(bmp.data.size()) + bmp.bmp_info_header.height * static_cast<uint32_t>(padding_row.size());
            }

            return true;
        }
        else {
            logger::error("Unable to open the input BMP image file.");
            return false;
        }
	}
}
