#include "text.h"

#include <algorithm>
#include <mutex>
#include <cmath>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "core/gl.h"
#include "math/glm.h"
#include "utils/shader_constants.h"
#include "utils/logger.h"
#include "utils/resources.h"


Font::Font()
{
	using namespace constants::shader;

	static std::once_flag prepareOnceFlag;
	std::call_once(prepareOnceFlag, []() {
		ShaderProgramManager::instance().load(internals::shaderFiles(freetype_font));
	});

	addCharacterRange(32, 127); // ASCII characters //
}

bool Font::load(std::string_view filepath, int pixelSize)
{
    if (pixelSize < 1 || pixelSize > CharactersTextureSize)
    {
        logger::warn("Invalid texture size '{}' in Font. Clamped between [{},{}]", pixelSize, 1, CharactersTextureSize);
        pixelSize = glm::clamp(pixelSize, 1, CharactersTextureSize);
    }

    FT_Library freeTypeLibrary;
    FT_Face freeTypeFace;

    auto ftError = FT_Init_FreeType(&freeTypeLibrary);
    if (ftError)
        return false;

    Path fontFilePath = resources::absolute(resources::fonts / filepath);

    ftError = FT_New_Face(freeTypeLibrary, fontFilePath.string().c_str(), 0, &freeTypeFace);
    if (ftError)
        return false;

    FT_Set_Pixel_Sizes(freeTypeFace, 0, pixelSize);
    _pixelSize = pixelSize;

    std::vector<unsigned char> textureData(CharactersTextureSize * CharactersTextureSize, 0);
    int currentPixelPositionRow = 0;
    int currentPixelPositionCol = 0;
    int rowHeight = 0;
    int currentRenderIndex = 0;
    std::unique_ptr<Texture> texture = std::make_unique<Texture>();

    std::vector<glm::vec2> vertData;
    std::vector<glm::vec2> texCoordData;

    auto finalizeTexture = [this, &texture, &textureData](bool createNext)
    {
        texture->createFromData(textureData.data(), CharactersTextureSize, CharactersTextureSize, Texture::Format::depth_component, true);
        texture->bind();
        texture->setFilter(Texture::MagnificationFilter::Bilinear);
        texture->setFilter(Texture::MinificationFilter::Nearest);
        texture->unbind();
        _textures.push_back(std::move(texture));
        if (createNext)
        {
            texture = std::make_unique<Texture>();
            memset(textureData.data(), 0, textureData.size());
        }
    };

    for (const auto& characterRange : _characterRanges)
    {
        for (auto c = characterRange.characterCodeFrom; c <= characterRange.characterCodeTo;)
        {
            FT_Load_Glyph(freeTypeFace, FT_Get_Char_Index(freeTypeFace, c), FT_LOAD_DEFAULT);
            FT_Render_Glyph(freeTypeFace->glyph, FT_RENDER_MODE_NORMAL);

            const auto* ptrBitmap = &freeTypeFace->glyph->bitmap;
            const int bmpWidth = ptrBitmap->width;
            const int bmpHeight = ptrBitmap->rows;
            const auto rowsLeft = CharactersTextureSize - currentPixelPositionRow;
            const auto colsLeft = CharactersTextureSize - currentPixelPositionCol;

            rowHeight = std::max(rowHeight, static_cast<int>(bmpHeight));

            if (colsLeft < bmpWidth)
            {
                currentPixelPositionCol = 0;
                currentPixelPositionRow += rowHeight + 1;
                rowHeight = 0;
                continue;
            }

            if (rowsLeft < bmpHeight)
            {
                finalizeTexture(true);
                currentPixelPositionCol = 0;
                currentPixelPositionRow = 0;
                rowHeight = 0;
                continue;
            }

            auto& charProps = _characterProperties[c]; // This also creates entry, if it does not exist
            charProps.characterCode = c;
            charProps.width = freeTypeFace->glyph->metrics.width >> 6;
            charProps.bearingX = freeTypeFace->glyph->metrics.horiBearingX >> 6;
            charProps.advanceX = freeTypeFace->glyph->metrics.horiAdvance >> 6;
            charProps.height = freeTypeFace->glyph->metrics.height >> 6;
            charProps.bearingY = freeTypeFace->glyph->metrics.horiBearingY >> 6;

            // If character is not renderable, e.g. space, then don't prepare rendering data for it
            if (bmpWidth == 0 && bmpHeight == 0) {
                charProps.renderIndex = -1;
                charProps.textureIndex = -1;
                c++;
                continue;
            }

            for (auto i = 0; i < bmpHeight; i++)
            {
                int globalRow = currentPixelPositionRow + i;
                int reversedRow = bmpHeight - i - 1;
                memcpy(textureData.data() + globalRow * CharactersTextureSize + currentPixelPositionCol, ptrBitmap->buffer + reversedRow * bmpWidth, bmpWidth);
            }

            // Setup vertices according to FreeType glyph metrics
            // You can find it here: https://www.freetype.org/freetype2/docs/glyphs/glyphs-3.html
            glm::vec2 vertices[] =
            {
                glm::vec2(static_cast<float>(charProps.bearingX), static_cast<float>(charProps.bearingY)),
                glm::vec2(static_cast<float>(charProps.bearingX), static_cast<float>(charProps.bearingY - charProps.height)),
                glm::vec2(static_cast<float>(bmpWidth + charProps.bearingX), static_cast<float>(charProps.bearingY)),
                glm::vec2(static_cast<float>(bmpWidth + charProps.bearingX), static_cast<float>(charProps.bearingY - charProps.height))
            };

            glm::vec2 textureCoordinates[] =
            {
                glm::vec2(static_cast<float>(currentPixelPositionCol) / static_cast<float>(CharactersTextureSize), static_cast<float>(currentPixelPositionRow + bmpHeight) / static_cast<float>(CharactersTextureSize)),
                glm::vec2(static_cast<float>(currentPixelPositionCol) / static_cast<float>(CharactersTextureSize), static_cast<float>(currentPixelPositionRow) / static_cast<float>(CharactersTextureSize)),
                glm::vec2(static_cast<float>(currentPixelPositionCol + bmpWidth) / static_cast<float>(CharactersTextureSize), static_cast<float>(currentPixelPositionRow + bmpHeight) / static_cast<float>(CharactersTextureSize)),
                glm::vec2(static_cast<float>(currentPixelPositionCol + bmpWidth) / static_cast<float>(CharactersTextureSize), static_cast<float>(currentPixelPositionRow) / static_cast<float>(CharactersTextureSize))
            };

            for (int i = 0; i < 4; i++)
            {
                vertData.push_back(vertices[i]);
                texCoordData.push_back(textureCoordinates[i]);
            }

            charProps.renderIndex = currentRenderIndex;
            charProps.textureIndex = static_cast<int>(_textures.size());
            currentPixelPositionCol += bmpWidth + 1;
            currentRenderIndex += 4;
            c++;
        }
    }

    // If there is a leftover texture after preparing the characters, add it to the list of textures
    if (currentPixelPositionRow > 0 || currentPixelPositionCol > 0) {
        finalizeTexture(false);
    }

    _vao.createAttribute(
        0,
        gl::VAO::Attribute::ComponentCount::Two,
        gl::DataType::Float,
        GL_FALSE,
        0,
        vertData,
        gl::VBO::Usage::StaticDraw
    );

    _vao.createAttribute(
        1,
        gl::VAO::Attribute::ComponentCount::Two,
        gl::DataType::Float,
        GL_FALSE,
        0,
        texCoordData,
        gl::VBO::Usage::StaticDraw
    );

    // Now we're done with loading, release FreeType structures
    FT_Done_Face(freeTypeFace);
    FT_Done_FreeType(freeTypeLibrary);

    _loaded = true;
    return true;
}

int Font::getTextWidth(std::string_view text, int pixelSize) const
{
    float result = 0.0f;
    float rowWidth = 0.0f;
    const auto usedPixelSize = pixelSize == -1 ? _pixelSize : pixelSize;
    const auto scale = static_cast<float>(usedPixelSize) / static_cast<float>(_pixelSize);

    // TODO: would be nice to handle invalid characters here as well
    for (int i = 0; i < static_cast<int>(text.length()); i++)
    {
        if (text[i] == '\n' || text[i] == '\r') {
            continue;
        }

        bool lastCharacterInRow = i == text.length() - 1 || text[i + 1] == '\n' || text[i + 1] == '\r';
        const auto& props = _characterProperties.at(text[i]);
        if (!lastCharacterInRow)
        {
            rowWidth += props.advanceX * scale;
            continue;
        }

        // Handle last character in a row in a special way + update the result
        rowWidth += (props.bearingX + props.width) * scale;
        result = std::max(result, rowWidth);
        rowWidth = 0.0f;
    }

    // Return ceiling of result
    return static_cast<int>(ceil(result));
}

int Font::getTextHeight(std::string_view text, int pixelSize) const
{
    const auto usedPixelSize = pixelSize == -1 ? _pixelSize : pixelSize;
    const auto scale = static_cast<float>(usedPixelSize) / static_cast<float>(_pixelSize);

    return static_cast<int>(ceil(usedPixelSize * scale));
}

void Font::destroy()
{
    _loaded = false;
    _textures.clear();
    _characterProperties.clear();
    _characterRanges.clear();
    _vao.destroy();
}
