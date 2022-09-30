#pragma once

#include <algorithm>

#include "core/gl.h"
#include "math/glm.h"


class Light
{
private:
	glm::vec3 _position = glm::vec3(0, 0, 0);
	glm::vec3 _color = glm::vec3(1, 1, 1);
	float _intensity = 1;
	bool _global = false;

public:
	constexpr Light() = default;
	constexpr Light(const Light&) = default;
	constexpr Light(Light&&) noexcept = default;
	constexpr ~Light() = default;

	constexpr Light& operator= (const Light&) = default;
	constexpr Light& operator= (Light&&) noexcept = default;


	constexpr void setPosition(const glm::vec3& position) { _position = position; }
	constexpr const glm::vec3& getPosition() const { return _position; }

	constexpr void setColor(const glm::vec3& color) { _color = color; }
	constexpr const glm::vec3& getColor() const { return _color; }

	constexpr void setIntensity(float intensity) { _intensity = intensity; }
	constexpr float getIntensity() const { return _intensity; }

	constexpr void setGlobal(bool flag) { _global = flag; }
	constexpr bool isGlobal() const { return _global; }
};



class ShaderLights
{
public:
	static constexpr std::size_t maxLights = 16;
	static constexpr float minIntensity = .01f;

private:
	

private:
	std::vector<Light> _lights;
	glm::vec3 _position = glm::vec3(0, 0, 0);

	std::vector<glm::vec3> _lightPositions;
	std::vector<glm::vec3> _lightColors;
	std::vector<GLfloat> _lightIntensities;
	std::vector<GLint> _lightGlobals;

public:
	constexpr ShaderLights() = default;
	constexpr ShaderLights(const ShaderLights&) = default;
	constexpr ShaderLights(ShaderLights&&) noexcept = default;
	constexpr ~ShaderLights() = default;

	constexpr ShaderLights& operator= (const ShaderLights&) = default;
	constexpr ShaderLights& operator= (ShaderLights&&) noexcept = default;


	constexpr void setPosition(const glm::vec3& position) { _position = position; }
	constexpr const glm::vec3& getPosition() const { return _position; }
	
	constexpr bool addLight(const Light& light)
	{
		if (light.isGlobal())
			_lights.push_back(light);
		else
		{
			float dist = glm::length(light.getPosition() - _position);
			float intensity = light.getIntensity() / (dist * dist);
			if (intensity >= minIntensity)
				_lights.push_back(light);
		}
	}

	constexpr void prepareAndSort()
	{
		
	}
};
