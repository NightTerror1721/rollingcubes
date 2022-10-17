#include "jpeg_decoder.h"

#include <JPEG/jpeglib.h>

#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "core/time.h"

namespace marengo
{
    namespace jpeg
    {
        std::vector<unsigned char> load(const std::string& fileName, size_t& width, size_t& height, size_t& pixelSize)
        {
            // Creating a custom deleter for the decompressInfo pointer
            // to ensure ::jpeg_destroy_compress() gets called even if
            // we throw out of this function.
            auto dt = [](::jpeg_decompress_struct* ds)
            {
                ::jpeg_destroy_decompress(ds);
            };
            std::unique_ptr<::jpeg_decompress_struct, decltype(dt)> decompressInfo(
                new ::jpeg_decompress_struct,
                dt
            );

            // Note this is a shared pointer as we can share this 
            // between objects which have copy constructed from each other
            auto m_errorMgr = std::make_shared<::jpeg_error_mgr>();

            // Using fopen here ( and in save() ) because libjpeg expects
            // a FILE pointer.
            // We store the FILE* in a unique_ptr so we can also use the custom
            // deleter here to ensure fclose() gets called even if we throw.
            auto fdt = [](FILE* fp)
            {
                fclose(fp);
            };
            std::unique_ptr<FILE, decltype(fdt)> infile(
                fopen(fileName.c_str(), "rb"),
                fdt
            );
            if (infile.get() == NULL)
            {
                throw std::runtime_error("Could not open " + fileName);
            }

            decompressInfo->err = ::jpeg_std_error(m_errorMgr.get());
            // Note this usage of a lambda to provide our own error handler
            // to libjpeg. If we do not supply a handler, and libjpeg hits
            // a problem, it just prints the error message and calls exit().
            m_errorMgr->error_exit = [](::j_common_ptr cinfo)
            {
                char jpegLastErrorMsg[JMSG_LENGTH_MAX];
                // Call the function pointer to get the error message
                (*(cinfo->err->format_message))
                    (cinfo, jpegLastErrorMsg);
                throw std::runtime_error(jpegLastErrorMsg);
            };
            ::jpeg_create_decompress(decompressInfo.get());

            // Read the file:
            ::jpeg_stdio_src(decompressInfo.get(), infile.get());

            int rc = ::jpeg_read_header(decompressInfo.get(), TRUE);
            if (rc != 1)
            {
                throw std::runtime_error(
                    "File does not seem to be a normal JPEG"
                );
            }
            ::jpeg_start_decompress(decompressInfo.get());

            auto m_width = decompressInfo->output_width;
            auto m_height = decompressInfo->output_height;
            auto m_pixelSize = decompressInfo->output_components;
            auto m_colourSpace = decompressInfo->out_color_space;

            width = m_width;
            height = m_height;
            pixelSize = m_pixelSize;

            size_t const row_stride = m_width * m_pixelSize;
            size_t const size = row_stride * m_height;

            //m_bitmapData.clear();
            //m_bitmapData.reserve(m_height);

            std::vector<unsigned char> bitmapData;
            bitmapData.resize(size);
            unsigned char* const begin_pdata = bitmapData.data();

            //Clock clock;

            while (decompressInfo->output_scanline < m_height)
            {
                //unsigned char* pdata = begin_pdata + (size - (row_stride * (decompressInfo->output_scanline + 1)));
                unsigned char* pdata = begin_pdata + (row_stride * decompressInfo->output_scanline);
                ::jpeg_read_scanlines(decompressInfo.get(), &pdata, 1);
            }

            /*while (decompressInfo->output_scanline < m_height)
            {
                

                std::vector<uint8_t> vec(row_stride);
                uint8_t* p = vec.data();
                ::jpeg_read_scanlines(decompressInfo.get(), &p, 1);
                m_bitmapData.push_back(vec);
            }*/
            //auto t = clock.getElapsedTime();
            //std::cout << "time: " << t.toSeconds() << std::endl;
            ::jpeg_finish_decompress(decompressInfo.get());

            return bitmapData;
        }

        Image::Image(const std::string& fileName)
        {
            // Creating a custom deleter for the decompressInfo pointer
            // to ensure ::jpeg_destroy_compress() gets called even if
            // we throw out of this function.
            auto dt = [](::jpeg_decompress_struct* ds)
            {
                ::jpeg_destroy_decompress(ds);
            };
            std::unique_ptr<::jpeg_decompress_struct, decltype(dt)> decompressInfo(
                new ::jpeg_decompress_struct,
                dt
            );

            // Note this is a shared pointer as we can share this 
            // between objects which have copy constructed from each other
            m_errorMgr = std::make_shared<::jpeg_error_mgr>();

            // Using fopen here ( and in save() ) because libjpeg expects
            // a FILE pointer.
            // We store the FILE* in a unique_ptr so we can also use the custom
            // deleter here to ensure fclose() gets called even if we throw.
            auto fdt = [](FILE* fp)
            {
                fclose(fp);
            };
            std::unique_ptr<FILE, decltype(fdt)> infile(
                fopen(fileName.c_str(), "rb"),
                fdt
            );
            if (infile.get() == NULL)
            {
                throw std::runtime_error("Could not open " + fileName);
            }

            decompressInfo->err = ::jpeg_std_error(m_errorMgr.get());
            // Note this usage of a lambda to provide our own error handler
            // to libjpeg. If we do not supply a handler, and libjpeg hits
            // a problem, it just prints the error message and calls exit().
            m_errorMgr->error_exit = [](::j_common_ptr cinfo)
            {
                char jpegLastErrorMsg[JMSG_LENGTH_MAX];
                // Call the function pointer to get the error message
                (*(cinfo->err->format_message))
                    (cinfo, jpegLastErrorMsg);
                throw std::runtime_error(jpegLastErrorMsg);
            };
            ::jpeg_create_decompress(decompressInfo.get());

            // Read the file:
            ::jpeg_stdio_src(decompressInfo.get(), infile.get());

            int rc = ::jpeg_read_header(decompressInfo.get(), TRUE);
            if (rc != 1)
            {
                throw std::runtime_error(
                    "File does not seem to be a normal JPEG"
                );
            }
            ::jpeg_start_decompress(decompressInfo.get());

            m_width = decompressInfo->output_width;
            m_height = decompressInfo->output_height;
            m_pixelSize = decompressInfo->output_components;
            m_colourSpace = decompressInfo->out_color_space;

            size_t row_stride = m_width * m_pixelSize;

            m_bitmapData.clear();
            m_bitmapData.reserve(m_height);

            //Clock clock;
            while (decompressInfo->output_scanline < m_height)
            {
                std::vector<uint8_t> vec(row_stride);
                uint8_t* p = vec.data();
                ::jpeg_read_scanlines(decompressInfo.get(), &p, 1);
                m_bitmapData.push_back(vec);
            }
            //auto t = clock.getElapsedTime();
            //std::cout << "time: " << t.toSeconds() << std::endl;
            ::jpeg_finish_decompress(decompressInfo.get());
        }

        // Copy constructor
        Image::Image(const Image& rhs)
        {
            m_errorMgr = rhs.m_errorMgr;
            m_bitmapData = rhs.m_bitmapData;
            m_width = rhs.m_width;
            m_height = rhs.m_height;
            m_pixelSize = rhs.m_pixelSize;
            m_colourSpace = rhs.m_colourSpace;
        }

        Image::~Image()
        {
        }

        void Image::save(const std::string& fileName, int quality) const
        {
            if (quality < 0)
            {
                quality = 0;
            }
            if (quality > 100)
            {
                quality = 100;
            }
            FILE* outfile = fopen(fileName.c_str(), "wb");
            if (outfile == NULL)
            {
                throw std::runtime_error(
                    "Could not open " + fileName + " for writing"
                );
            }
            // Creating a custom deleter for the compressInfo pointer
            // to ensure ::jpeg_destroy_compress() gets called even if
            // we throw out of this function.
            auto dt = [](::jpeg_compress_struct* cs)
            {
                ::jpeg_destroy_compress(cs);
            };
            std::unique_ptr<::jpeg_compress_struct, decltype(dt)> compressInfo(
                new ::jpeg_compress_struct,
                dt);
            ::jpeg_create_compress(compressInfo.get());
            ::jpeg_stdio_dest(compressInfo.get(), outfile);
            compressInfo->image_width = JDIMENSION(m_width);
            compressInfo->image_height = JDIMENSION(m_height);
            compressInfo->input_components = int(m_pixelSize);
            compressInfo->in_color_space =
                static_cast<::J_COLOR_SPACE>(m_colourSpace);
            compressInfo->err = ::jpeg_std_error(m_errorMgr.get());
            ::jpeg_set_defaults(compressInfo.get());
            ::jpeg_set_quality(compressInfo.get(), quality, TRUE);
            ::jpeg_start_compress(compressInfo.get(), TRUE);
            for (auto const& vecLine : m_bitmapData)
            {
                ::JSAMPROW rowPtr[1];
                // Casting const-ness away here because the jpeglib
                // call expects a non-const pointer. It presumably
                // doesn't modify our data.
                rowPtr[0] = const_cast<::JSAMPROW>(vecLine.data());
                ::jpeg_write_scanlines(
                    compressInfo.get(),
                    rowPtr,
                    1
                );
            }
            ::jpeg_finish_compress(compressInfo.get());
            fclose(outfile);
        }

        void Image::savePpm(const std::string& fileName) const
        {
            std::ofstream ofs(fileName, std::ios::out | std::ios::binary);
            if (!ofs)
            {
                throw std::runtime_error(
                    "Could not open " + fileName + " for saving"
                );
            }
            // Write the header
            ofs << "P6 " << m_width << " " << m_height << " 255\n";
            for (auto& v : m_bitmapData)
            {
                ofs.write(reinterpret_cast<const char*>(v.data()), v.size());
            }
            ofs.close();
        }

        std::vector<uint8_t> Image::getPixel(size_t x, size_t y) const
        {
            if (y >= m_bitmapData.size())
            {
                throw std::out_of_range("Y value too large");
            }
            if (x >= m_bitmapData[0].size() / m_pixelSize)
            {
                throw std::out_of_range("X value too large");
            }
            std::vector<uint8_t> vec;
            for (size_t n = 0; n < m_pixelSize; ++n)
            {
                vec.push_back(m_bitmapData[y][x * m_pixelSize + n]);
            }
            return vec;
        }

        uint8_t Image::getLuminance(size_t x, size_t y) const
        {
            auto vec = getPixel(x, y);
            if (vec.size() == 1)
            {   // monochrome image
                return vec[0];
            }
            if (vec.size() == 3)
            {
                // fast approximation of luminance:
                return static_cast<uint8_t>(
                    (vec[0] * 2 + vec[1] * 3 + vec[2]) / 6
                    );
            }
            return 0;
        }

        std::vector<uint8_t>
            Image::getAverage(size_t x, size_t y, size_t boxSize) const
        {
            if (boxSize > m_width)
            {
                throw std::out_of_range("Box size is greater than image width");
            }
            if (boxSize > m_height)
            {
                throw std::out_of_range("Box size is greater than image height");
            }
            if (x + boxSize >= m_width)
            {
                x = m_width - boxSize;
            }
            if (y + boxSize >= m_height)
            {
                y = m_height - boxSize;
            }
            // running totals
            size_t r{ 0 }; // we just use this one for mono images
            size_t g{ 0 };
            size_t b{ 0 };
            for (size_t row = y; row < y + boxSize; ++row)
            {
                for (size_t col = x; col < x + boxSize; ++col)
                {
                    auto vec = getPixel(col, row);
                    r += vec[0];
                    if (vec.size() == 3)
                    {
                        g += vec[1];
                        b += vec[2];
                    }
                }
            }
            std::vector<uint8_t> retVec;
            r /= (boxSize * boxSize);
            retVec.push_back(uint8_t(r));
            if (m_pixelSize == 3)
            {
                g /= (boxSize * boxSize);
                retVec.push_back(uint8_t(g));
                b /= (boxSize * boxSize);
                retVec.push_back(uint8_t(b));
            }
            return retVec;
        }

        void Image::shrink(size_t newWidth)
        {
            if (newWidth >= m_width)
            {
                return;
            }

            if (newWidth == 0)
            {
                throw std::out_of_range("New width cannot be zero");
            }

            // We process the original bitmap line by line rather than
            // calling getAverage() on every (new) pixel to ensure we make the
            // most of data already in existing cache lines & attempt to
            // allow branch prediction to work optimally. This has resulted
            // in a three-times speedup when shrinking a 21Mpx file.

            float scaleFactor = static_cast<float>(newWidth) / m_width;
            size_t newHeight = size_t(scaleFactor * m_height);
            std::vector<std::vector<uint8_t>> vecNewBitmap;
            vecNewBitmap.reserve(newHeight);

            // Yes, I probably could do a rolling average
            std::vector<size_t> runningTotals(newWidth * m_pixelSize);
            std::vector<size_t> runningCounts(newWidth * m_pixelSize);
            size_t oldRow = 0;
            for (size_t row = 0; row < m_height; ++row)
            {
                for (size_t col = 0; col < m_width * m_pixelSize; ++col)
                {
                    size_t idx = size_t(scaleFactor * col);
                    runningTotals[idx] += m_bitmapData[row][col];
                    ++runningCounts[idx];
                }
                if (static_cast<size_t>(scaleFactor * row) > oldRow)
                {
                    oldRow = size_t(scaleFactor * row);
                    std::vector<uint8_t> newLine;
                    newLine.reserve(newWidth * m_pixelSize);
                    for (size_t i = 0; i < newWidth * m_pixelSize; ++i)
                    {
                        newLine.push_back(uint8_t(runningTotals[i] / runningCounts[i]));
                        runningTotals[i] = 0;
                        runningCounts[i] = 0;
                    }
                    vecNewBitmap.push_back(newLine);
                }
            }
            m_bitmapData = vecNewBitmap;
            m_height = m_bitmapData.size();
            m_width = m_bitmapData[0].size() / m_pixelSize;
        }

        void Image::expand(size_t newWidth)
        {
            if (newWidth <= m_width)
            {
                return;
            }

            float scaleFactor = static_cast<float>(newWidth) / m_width;
            size_t newHeight = size_t(scaleFactor * m_height);
            std::vector<std::vector<uint8_t>> vecNewBitmap;
            vecNewBitmap.reserve(newHeight);

            for (size_t row = 0; row < newHeight; ++row)
            {
                size_t oldRow = size_t(row / scaleFactor);
                std::vector<uint8_t> vecNewLine(newWidth * m_pixelSize);
                for (size_t col = 0; col < newWidth; ++col)
                {
                    size_t oldCol = size_t(col / scaleFactor);
                    for (size_t n = 0; n < m_pixelSize; ++n)
                    {
                        vecNewLine[col * m_pixelSize + n] =
                            m_bitmapData[oldRow][oldCol * m_pixelSize + n];
                    }
                }
                vecNewBitmap.push_back(vecNewLine);
            }
            m_bitmapData = vecNewBitmap;
            m_height = m_bitmapData.size();
            m_width = m_bitmapData[0].size() / m_pixelSize;
        }

        void Image::resize(size_t newWidth)
        {
            if (newWidth < m_width)
            {
                shrink(newWidth);
            }
            else if (newWidth > m_width)
            {
                expand(newWidth);
            }
        }


    } // namespace jpeg
} // namespace marengo
