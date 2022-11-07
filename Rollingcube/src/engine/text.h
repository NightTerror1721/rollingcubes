#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

#include "core/gl.h"
#include "core/render.h"

#include "utils/string_utils.h"
#include "utils/unicode.h"

#include "texture.h"
#include "shader.h"
#include "camera.h"





class Font
{
public:
    static constexpr int CharactersTextureSize = 512;

private:
	struct CharacterProperties
	{
        int characterCode; // What character code these properties belong to

        // Following properties come from FreeType directly
        int width;
        int height;
        int advanceX;
        int bearingX;
        int bearingY;

        // These are our properties used for rendering
        int renderIndex;
        int textureIndex;
	};

    struct CharacterRange
    {
        unsigned int characterCodeFrom; // Start of unicode range
        unsigned int characterCodeTo; // End of unicode range
    };

private:
	bool _loaded = false;
    std::vector<CharacterRange> _characterRanges;
    int _pixelSize = 0;
    glm::vec4 _color = { 1, 1, 1, 1 };

    std::vector<std::unique_ptr<Texture>> _textures;
    std::unordered_map<int, CharacterProperties> _characterProperties;
    gl::VAO _vao;

    mutable ShaderProgram::Ref _shaderCache = {};

public:
    Font(const Font&) = delete;
    Font& operator= (const Font&) = delete;

    inline ~Font() { destroy(); }

public:
    Font();

    bool load(std::string_view filepath, int pixelSize);

    int getTextWidth(std::string_view text, int pixelSize = -1) const;
    int getTextHeight(std::string_view text, int pixelSize = -1) const;

    void destroy();

public:
    constexpr bool isLoaded() const { return _loaded; }

    constexpr void addCharacterRange(unsigned int from, unsigned int to) { _characterRanges.push_back({ from, to }); }

    constexpr void setColor(const glm::vec4& color) { _color = color; }
    constexpr void setColor(const glm::vec3& color) { _color = { color.r, color.g, color.b, _color.a }; }
    constexpr const glm::vec4& getColor() const { return _color; }

    template <typename... _ArgsTys>
    inline void print(const Camera& cam, int x, int y, std::string_view text, _ArgsTys&&... args) const
    {
        _print<std::string_view::value_type>(cam, x, y, utils::str::format(text, std::forward<_ArgsTys>(args)...), -1);
    }

    template <typename... _ArgsTys>
    inline void print(const Camera& cam, int x, int y, int pixelSize, std::string_view text, _ArgsTys&&... args) const
    {
        _print<std::string_view::value_type>(cam, x, y, utils::str::format(text, std::forward<_ArgsTys>(args)...), pixelSize);
    }

    inline void print(const Camera& cam, int x, int y, const UnicodeString& text)
    {
        _print<UnicodeString::value_type>(cam, x, y, text, -1);
    }

    inline void print(const Camera& cam, int x, int y, int pixelSize, const UnicodeString& text)
    {
        _print<UnicodeString::value_type>(cam, x, y, text, pixelSize);
    }

private:
    template <UtfChar _CharTy>
    void _print(const Camera& cam, int x, int y, std::basic_string_view<_CharTy> text, int pixelSize) const;

private:
    inline ShaderProgram::Ref getFreetypeFontShader() const
    {
        if (!_shaderCache)
            _shaderCache = ShaderProgramManager::instance().getFreetypeFontShaderProgram();
        return _shaderCache;
    }
};





template <UtfChar _CharTy>
void Font::_print(const Camera& cam, int x, int y, std::basic_string_view<_CharTy> text, int pixelSize) const
{
    if (!_loaded)
        return;

    glDisable(GL_DEPTH_TEST);
    glDepthMask(0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto shaderProgram = getFreetypeFontShader();
    shaderProgram->use();
    shaderProgram["projection"] = cam.getProjectionMatrix();
    shaderProgram["color"] = _color;

    glm::vec2 currentPos(x, y);
    const auto usedPixelSize = pixelSize == -1 ? _pixelSize : pixelSize;
    auto lastBoundTextureIndex = -1;
    const auto scale = static_cast<float>(usedPixelSize) / static_cast<float>(_pixelSize);

    _vao.bind();
    for (const auto& c : text)
    {
        if (c == '\n' || c == '\r')
        {
            currentPos.x = static_cast<float>(x);
            currentPos.y -= static_cast<float>(usedPixelSize);
            continue;
        }

        // If we somehow stumble upon unknown character, ignore it
        if (_characterProperties.count(c) == 0) {
            continue;
        }

        const auto& props = _characterProperties.at(c);
        if (props.renderIndex != -1)
        {
            if (lastBoundTextureIndex != props.textureIndex)
            {
                lastBoundTextureIndex = props.textureIndex;
                _textures.at(props.textureIndex)->activate();
            }

            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(currentPos.x, currentPos.y, 0.0f));
            model = glm::scale(model, glm::vec3(scale, scale, 1.0f));
            shaderProgram["model"] = model;
            glDrawArrays(GL_TRIANGLE_STRIP, props.renderIndex, 4);
        }

        currentPos.x += props.advanceX * scale;
    }
    _vao.unbind();

    glDisable(GL_BLEND);
    glDepthMask(1);
    glEnable(GL_DEPTH_TEST);
}
