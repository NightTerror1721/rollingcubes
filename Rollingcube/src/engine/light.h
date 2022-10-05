#pragma once

#include <algorithm>
#include <memory>
#include <concepts>
#include <ranges>

#include <unordered_map>
#include <unordered_set>
#include <queue>

#include "core/gl.h"
#include "math/glm.h"
#include "utils/utils.h"
#include "utils/color_channels.h"


namespace // private //
{
	class AbstractLight : public ColorChannels
	{
	protected:
		float _intensity = 1;

	public:
		constexpr AbstractLight(const AbstractLight&) = default;
		constexpr AbstractLight(AbstractLight&&) noexcept = default;
		constexpr ~AbstractLight() = default;

		constexpr AbstractLight& operator= (const AbstractLight&) = default;
		constexpr AbstractLight& operator= (AbstractLight&&) noexcept = default;


		constexpr AbstractLight() : ColorChannels({ 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }) {}


		constexpr void setIntensity(float intensity) { _intensity = intensity; }
		constexpr float getIntensity() const { return _intensity; }
	};
}


class Light : public AbstractLight
{
public:
	using ManagerId = std::size_t;
	static constexpr ManagerId invalid_manager_id = 0;

	using ListenerId = std::uint64_t;
	static constexpr ListenerId invalid_listener_id = 0;

protected:
	glm::vec3 _position = glm::vec3(0, 0, 0);

	float _constantAttenuation = 0;
	float _linearAttenuation = 0;
	float _quadraticAttenuation = 1;

public:
	constexpr Light() = default;
	constexpr Light(const Light&) = default;
	constexpr Light(Light&&) noexcept = default;
	constexpr ~Light() = default;

	constexpr Light& operator= (const Light&) = default;
	constexpr Light& operator= (Light&&) noexcept = default;


	constexpr void setPosition(const glm::vec3& position) { _position = position; }
	constexpr const glm::vec3& getPosition() const { return _position; }


	constexpr void setConstantAttenuation(float attenuationFactor) { _constantAttenuation = attenuationFactor; }
	constexpr float getConstantAttenuation() const { return _constantAttenuation; }

	constexpr void setLinearAttenuation(float attenuationFactor) { _linearAttenuation = attenuationFactor; }
	constexpr float getLinearAttenuation() const { return _linearAttenuation; }

	constexpr void setQuadraticAttenuation(float attenuationFactor) { _quadraticAttenuation = attenuationFactor; }
	constexpr float getQuadraticAttenuation() const { return _quadraticAttenuation; }
};


class DirectionalLight : public AbstractLight
{
protected:
	glm::vec3 _direction = glm::vec3(0, 0, 0);

public:
	constexpr DirectionalLight() = default;
	constexpr DirectionalLight(const DirectionalLight&) = default;
	constexpr DirectionalLight(DirectionalLight&&) noexcept = default;
	constexpr ~DirectionalLight() = default;

	constexpr DirectionalLight& operator= (const DirectionalLight&) = default;
	constexpr DirectionalLight& operator= (DirectionalLight&&) noexcept = default;


	constexpr void setDirection(const glm::vec3& direction) { _direction = direction; }
	constexpr const glm::vec3& getDirection() const { return _direction; }
};


class SpotLight : public AbstractLight
{
private:
	glm::vec3 _position = glm::vec3(0, 0, 0);
	glm::vec3 _direction = glm::vec3(0, 0, 0);

	float _cutOff = 12.5f;
	float _outerCutOff = 17.5f;

	float _constantAttenuation = 0;
	float _linearAttenuation = 0;
	float _quadraticAttenuation = 1;

public:
	constexpr SpotLight() = default;
	constexpr SpotLight(const SpotLight&) = default;
	constexpr SpotLight(SpotLight&&) noexcept = default;
	constexpr ~SpotLight() = default;

	constexpr SpotLight& operator= (const SpotLight&) = default;
	constexpr SpotLight& operator= (SpotLight&&) noexcept = default;


	constexpr void setPosition(const glm::vec3& position) { _position = position; }
	constexpr const glm::vec3& getPosition() const { return _position; }


	constexpr void setDirection(const glm::vec3& direction) { _direction = direction; }
	constexpr const glm::vec3& getDirection() const { return _direction; }


	constexpr void setCutOff(float cutOff) { _cutOff = cutOff; }
	constexpr float getCutOff() const { return _cutOff; }

	constexpr void setOuterCutOff(float cutOff) { _outerCutOff = cutOff; }
	constexpr float getOuterCutOff() const { return _outerCutOff; }


	constexpr void setConstantAttenuation(float attenuationFactor) { _constantAttenuation = attenuationFactor; }
	constexpr float getConstantAttenuation() const { return _constantAttenuation; }

	constexpr void setLinearAttenuation(float attenuationFactor) { _linearAttenuation = attenuationFactor; }
	constexpr float getLinearAttenuation() const { return _linearAttenuation; }

	constexpr void setQuadraticAttenuation(float attenuationFactor) { _quadraticAttenuation = attenuationFactor; }
	constexpr float getQuadraticAttenuation() const { return _quadraticAttenuation; }
};



class LightManager;



class ShaderLights
{
public:
	static constexpr std::size_t maxLights = 8;
	static constexpr float minIntensity = .01f;

	friend LightManager;

private:
	LightManager* _manager;
	Light::ListenerId _listenerId = 0;
	glm::vec3 _position = glm::vec3(0, 0, 0);
	bool _needBuild = true;

private:
	std::vector<Light::ManagerId> _lights;

	std::vector<glm::vec3> _lightPositions;
	std::vector<glm::vec3> _lightDiffuseColors;
	std::vector<glm::vec3> _lightAmbientColors;
	std::vector<glm::vec3> _lightSpecularColors;
	std::vector<GLfloat> _lightConstants;
	std::vector<GLfloat> _lightLinears;
	std::vector<GLfloat> _lightQuadratics;
	std::vector<GLfloat> _lightIntensities;

public:
	ShaderLights(const ShaderLights&) = delete;
	ShaderLights(ShaderLights&&) noexcept = default;

	ShaderLights& operator= (const ShaderLights&) = delete;
	ShaderLights& operator= (ShaderLights&&) noexcept = default;

	inline ShaderLights(LightManager& manager) : _manager(std::addressof(manager)) {}

public:
	~ShaderLights();

	inline void setPosition(const glm::vec3& position) { _position = position; }
	inline const glm::vec3& getPosition() const { return _position; }

	void build();

	const Light& getLight(GLint index) const;


	inline bool empty() const { return _lights.empty(); }
	inline std::size_t size() const { return _lights.size(); }

	inline const std::vector<glm::vec3>& getLightsPositions() const { return _lightPositions; }
	inline const std::vector<glm::vec3>& getLightsDiffuseColors() const { return _lightDiffuseColors; }
	inline const std::vector<glm::vec3>& getLightsAmbientColors() const { return _lightAmbientColors; }
	inline const std::vector<glm::vec3>& getLightsSpecularColors() const { return _lightSpecularColors; }
	inline const std::vector<GLfloat>& getLightsConstantAttenuations() const { return _lightConstants; }
	inline const std::vector<GLfloat>& getLightsLinearAttenuations() const { return _lightLinears; }
	inline const std::vector<GLfloat>& getLightsQuadraticAttenuations() const { return _lightQuadratics; }
	inline const std::vector<GLfloat>& getLightsIntensities() const { return _lightIntensities; }


	inline const Light& operator[] (GLint index) const { return getLight(index); }
};




class LightManager
{
public:
	friend ShaderLights;

private:
	struct Allocator
	{
		Light::ManagerId id;
		std::shared_ptr<Light> light;
		std::unordered_set<Light::ListenerId> listeners;
	};

	static constexpr std::size_t managerIdToIndex(Light::ManagerId id) { return id - 1; }
	static constexpr Light::ManagerId indexToManagerId(std::size_t id) { return id + 1; }

private:
	IndexedVector<Allocator> _lights;
	IndexedVector<std::shared_ptr<ShaderLights>> _listeners;

public:
	LightManager() = default;
	LightManager(const LightManager&) = delete;
	LightManager(LightManager&&) noexcept = default;
	~LightManager() = default;

	LightManager& operator= (const LightManager&) = delete;
	LightManager& operator= (LightManager&&) noexcept = default;


	std::shared_ptr<ShaderLights> createShaderLights();

	Light::ManagerId createNewLight(const Light& initial_light_data = Light());

	void updateLight(Light::ManagerId id, const Light& light);

	inline const Light& getLight(Light::ManagerId id) const { return *_lights[id].light.get(); }


	inline const Light& operator[] (Light::ManagerId id) const { return *_lights[id].light.get(); }

private:
	void destroyShaderLights(Light::ListenerId id);
	void releaseLightListener(Light::ManagerId lightId, Light::ListenerId listenerId);
};



inline const Light& ShaderLights::getLight(GLint index) const { return _manager->getLight(_lights[index]); }
