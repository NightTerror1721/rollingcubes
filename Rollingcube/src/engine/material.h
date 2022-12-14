#pragma once

#include <compare>
#include <memory>

#include "utils/color_channels.h"
#include "math/glm.h"
#include "math/color.h"
#include "texture.h"


class Material : public ColorChannels
{
public:
	using Ref = Reference<Material>;
	using ConstRef = ConstReference<Material>;

private:
	Texture::Ref _diffuseTexture = nullptr;
	Texture::Ref _specularTexture = nullptr;
	Texture::Ref _normalsTexture = nullptr;

	float _shininess = 5;
	float _opacity = 1;

public:
	Material(const Material&) = default;
	Material(Material&&) noexcept = default;
	~Material() = default;

	Material& operator= (const Material&) = default;
	Material& operator= (Material&&) noexcept = default;

	bool operator== (const Material&) const = default;


	inline Material() : ColorChannels({ 1, 1, 1 }, {}, {}) {}


	inline void setDiffuseTexture(Texture::Ref texture) { _diffuseTexture = texture; }
	inline Texture::Ref getDiffuseTexture() const { return _diffuseTexture; }

	inline void setSpecularTexture(Texture::Ref texture) { _specularTexture = texture; }
	inline Texture::Ref getSpecularTexture() const { return _specularTexture; }

	inline void setNormalsTexture(Texture::Ref texture) { _normalsTexture = texture; }
	inline Texture::Ref getNormalsTexture() const { return _normalsTexture; }


	inline void setShininess(float shininess) { _shininess = shininess; }
	inline float getShininess() const { return _shininess; }

	inline void setOpacity(float opacity) { _opacity = glm::clamp(opacity, 0.f, 1.f); }
	inline float getOpacity() const { return _opacity; }
	inline bool hasTransparency() const { return _opacity < 1; }


	inline void bindTextures() const
	{
		if (_diffuseTexture != nullptr)
			_diffuseTexture->activate(0);
		else
			Texture::deactivate(0);

		if (_specularTexture != nullptr)
			_specularTexture->activate(1);
		else if (_diffuseTexture != nullptr)
			_diffuseTexture->activate(1);
		else
			Texture::deactivate(1);

		if (_normalsTexture != nullptr)
			_normalsTexture->activate(2);
		else
			Texture::deactivate(2);
	}

	inline void unbindTextures() const
	{
		if (_diffuseTexture != nullptr)
			_diffuseTexture->unbind();

		if (_specularTexture != nullptr)
			_specularTexture->unbind();

		if (_normalsTexture != nullptr)
			_normalsTexture->unbind();
	}
};
