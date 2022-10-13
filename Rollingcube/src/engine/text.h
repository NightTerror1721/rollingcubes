#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

#include "core/gl.h"
#include "core/render.h"
#include "utils/string_utils.h"
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
        _print(cam, x, y, utils::str::format(text, std::forward<_ArgsTys>(args)...), -1);
    }

    template <typename... _ArgsTys>
    inline void print(const Camera& cam, int x, int y, int pixelSize, std::string_view text, _ArgsTys&&... args) const
    {
        _print(cam, x, y, utils::str::format(text, std::forward<_ArgsTys>(args)...), pixelSize);
    }

private:
    void _print(const Camera& cam, int x, int y, std::string_view text, int pixelSize) const;

private:
    inline ShaderProgram::Ref getFreetypeFontShader() const
    {
        if (!_shaderCache)
            _shaderCache = ShaderProgramManager::instance().getFreetypeFontShaderProgram();
        return _shaderCache;
    }
};
