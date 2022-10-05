#pragma once

#include "math/glm.h"


class ColorChannels
{
protected:
	glm::vec3 _ambientColor = {};
	glm::vec3 _diffuseColor = {};
	glm::vec3 _specularColor = {};

public:
	constexpr ColorChannels() = default;
	constexpr ColorChannels(const ColorChannels&) = default;
	constexpr ColorChannels(ColorChannels&&) noexcept = default;
	constexpr ~ColorChannels() = default;

	constexpr ColorChannels& operator= (const ColorChannels&) = default;
	constexpr ColorChannels& operator= (ColorChannels&&) noexcept = default;

	constexpr bool operator== (const ColorChannels&) const = default;


	constexpr ColorChannels(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular) :
		_ambientColor(ambient), _diffuseColor(diffuse), _specularColor(specular)
	{}


	constexpr void setDiffuseColor(const glm::vec3& color) { _diffuseColor = color; }
	constexpr const glm::vec3& getDiffuseColor() const { return _diffuseColor; }

	constexpr void setAmbientColor(const glm::vec3& color) { _ambientColor = color; }
	constexpr const glm::vec3& getAmbientColor() const { return _ambientColor; }

	constexpr void setSpecularColor(const glm::vec3& color) { _specularColor = color; }
	constexpr const glm::vec3& getSpecularColor() const { return _specularColor; }


	constexpr void setColor(const glm::vec3& color)
	{
		_diffuseColor = color;
		_ambientColor = color;
		_specularColor = color;
	}
};
