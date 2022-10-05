#pragma once

#include <compare>
#include <memory>

#include "utils/color_channels.h"
#include "math/glm.h"
#include "math/color.h"
#include "texture.h"


class Material : public ColorChannels
{
private:
	std::shared_ptr<Texture> _diffuseTexture = nullptr;
	std::shared_ptr<Texture> _specularTexture = nullptr;

	float _shininess = 5;

public:
	Material(const Material&) = default;
	Material(Material&&) noexcept = default;
	~Material() = default;

	Material& operator= (const Material&) = default;
	Material& operator= (Material&&) noexcept = default;

	bool operator== (const Material&) const = default;


	inline Material() : ColorChannels({ 1, 1, 1 }, {}, {}) {}


	inline void setDiffuseTexture(const std::shared_ptr<Texture>& texture) { _diffuseTexture = texture; }
	inline const std::shared_ptr<Texture>& getDiffuseTexture() const { return _diffuseTexture; }

	inline void setSpecularTexture(const std::shared_ptr<Texture>& texture) { _specularTexture = texture; }
	inline const std::shared_ptr<Texture>& getSpecularTexture() const { return _specularTexture; }


	inline void setShininess(float shininess) { _shininess = shininess; }
	inline float getShininess() const { return _shininess; }


	inline void bindTextures()
	{
		if (_diffuseTexture != nullptr)
			_diffuseTexture->bind();

		if (_specularTexture != nullptr)
			_specularTexture->bind();
	}

	inline void unbindTextures()
	{
		if (_diffuseTexture != nullptr)
			_diffuseTexture->unbind();

		if (_specularTexture != nullptr)
			_specularTexture->unbind();
	}
};
