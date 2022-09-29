#pragma once

#include <compare>

#include "math/glm.h"
#include "math/color.h"


class Material
{
private:
	glm::vec3 _diffuseColor = { 1, 1, 1 };
	glm::vec3 _ambientColor = { 0, 0, 0 };
	glm::vec3 _specularColor = { 0, 0, 0 };

	float _diffuseCoeficient = 1;
	float _ambientCoeficient = 1;
	float _specularCoeficient = 1;

	float _shininess = 5;

public:
	constexpr Material() = default;
	constexpr Material(const Material&) = default;
	constexpr Material(Material&&) noexcept = default;
	constexpr ~Material() = default;

	constexpr Material& operator= (const Material&) = default;
	constexpr Material& operator= (Material&&) noexcept = default;

	constexpr bool operator== (const Material&) const = default;


	constexpr void setDiffuseColor(const glm::vec3& color) { _diffuseColor = color; }
	constexpr glm::vec3 getDiffuseColor() const { return _diffuseColor; }

	constexpr void setAmbientColor(const glm::vec3& color) { _ambientColor = color; }
	constexpr glm::vec3 getAmbientColor() const { return _ambientColor; }

	constexpr void setSpecularColor(const glm::vec3& color) { _specularColor = color; }
	constexpr glm::vec3 getSpecularColor() const { return _specularColor; }


	constexpr void setDiffuseCoeficient(float coeficient) { _diffuseCoeficient = coeficient; }
	constexpr float getDiffuseCoeficient() const { return _diffuseCoeficient; }

	constexpr void setAmbientCoeficient(float coeficient) { _ambientCoeficient = coeficient; }
	constexpr float getAmbientCoeficient() const { return _ambientCoeficient; }

	constexpr void setSpecularCoeficient(float coeficient) { _specularCoeficient = coeficient; }
	constexpr float getSpecularCoeficient() const { return _specularCoeficient; }


	constexpr void setShininess(float shininess) { _shininess = shininess; }
	constexpr float getShininess() const { return _shininess; }
};
