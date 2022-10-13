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


	constexpr float computeAttenuation(float distance)
	{
		return 1.0f / (_constantAttenuation + _linearAttenuation * distance + _quadraticAttenuation * (distance * distance));
	}

	constexpr float computeAttenuatedIntensity(float distance)
	{
		return _intensity * computeAttenuation(distance);
	}

	inline float computeAttenuatedIntensityFrom(const glm::vec3& position)
	{
		return computeAttenuatedIntensity(glm::length(_position - position));
	}
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


using StaticLightId = std::size_t;

class StaticLightManager;


class StaticLightContainer
{
public:
	friend StaticLightManager;

	static constexpr std::size_t maxStaticLights = 8;

private:
	static constexpr float minIntensity = 0.01f;

private:
	std::shared_ptr<StaticLightManager> _manager = nullptr;
	std::vector<std::shared_ptr<Light>> _lights;
	glm::vec3 _position = { 0, 0, 0 };
	std::uint64_t _buildVersion = 0;

public:
	StaticLightContainer() = default;
	StaticLightContainer(const StaticLightContainer&) = default;
	StaticLightContainer(StaticLightContainer&&) noexcept = default;
	~StaticLightContainer() = default;

	StaticLightContainer& operator= (const StaticLightContainer&) = default;
	StaticLightContainer& operator= (StaticLightContainer&&) noexcept = default;

public:
	inline bool empty() const { return _lights.empty(); }
	inline std::size_t size() const { return _lights.size(); }

	inline const Light& at(std::size_t index) const { return *_lights.at(index); }

	inline const Light& operator[] (std::size_t index) const { return *_lights[index]; }

	inline const glm::vec3& getPosition() const { return _position; }

	inline void setPosition(const glm::vec3& position)
	{
		auto old = _position;
		_position = position;
		if (old != position)
			_buildVersion = 0;
	}

	inline void unlink()
	{
		_lights.clear();
		_manager.reset();
		_buildVersion = 0;
	}

	inline void link(const std::shared_ptr<StaticLightManager>& manager)
	{
		unlink();
		_manager = manager;
	}

	void update();

private:
	void build();
};


class StaticLightManager
{
public:
	friend StaticLightContainer;

private:
	std::unordered_map<StaticLightId, std::shared_ptr<Light>> _lights;
	std::queue<StaticLightId> _unusedIds;
	StaticLightId _nextId = 1;
	std::uint64_t _buildVersion = 1;

public:
	StaticLightManager() noexcept = default;
	StaticLightManager(const StaticLightManager&) = delete;
	StaticLightManager(StaticLightManager&&) noexcept = default;
	~StaticLightManager() = default;

	StaticLightManager& operator= (const StaticLightManager&) = delete;
	StaticLightManager& operator= (StaticLightManager&&) noexcept = default;

public:
	inline StaticLightId createNewLight(const Light& initialLight)
	{
		StaticLightId id;
		if (!_unusedIds.empty())
		{
			id = _unusedIds.front();
			_unusedIds.pop();
		}
		else
			id = _nextId++;

		_lights.insert({ id, std::make_shared<Light>(initialLight) });
		_buildVersion++;
		return id;
	}

	inline void destroyLight(StaticLightId lightId)
	{
		auto it = _lights.find(lightId);
		if (it != _lights.end())
		{
			_lights.erase(it);
			_unusedIds.push(lightId);
			_buildVersion++;
		}
	}

	inline void updateLight(StaticLightId id, const Light& light)
	{
		auto it = _lights.find(id);
		if (it != _lights.end())
		{
			glm::vec3 oldPos = it->second->getPosition();
			*it->second = light;
			if (oldPos != it->second->getPosition())
				_buildVersion++;
		}
	}

	inline const std::shared_ptr<Light>& getLight(StaticLightId lightId) const
	{
		auto it = _lights.find(lightId);
		if (it != _lights.end())
			return it->second;

		static const std::shared_ptr<Light> invalid_ptr = nullptr;
		return invalid_ptr;
	}

	inline const std::shared_ptr<Light>& operator[] (StaticLightId lightId) const { return getLight(lightId); }
};

inline void StaticLightContainer::build()
{
	struct LightIntensity
	{
		std::shared_ptr<Light>* light;
		float intensity;

		constexpr bool operator== (const LightIntensity& right) const { return intensity == right.intensity; };
		constexpr auto operator<=> (const LightIntensity& right) const { return intensity <=> right.intensity; };
	};

	if (_manager != nullptr)
	{
		_lights.clear();
		std::vector<LightIntensity> potentialLights;
		for (auto& light : _manager->_lights)
		{
			float intensity = light.second->computeAttenuatedIntensityFrom(_position);
			if (intensity >= minIntensity)
				potentialLights.push_back({ std::addressof(light.second), intensity });
		}

		if (!potentialLights.empty())
		{
			const std::size_t len = std::min(potentialLights.size(), maxStaticLights);
			_lights.resize(len);
			
			std::sort(potentialLights.begin(), potentialLights.end());
			for (std::size_t i = 0; i < len; ++i)
				_lights[i] = *potentialLights[i].light;
		}

		_buildVersion = _manager->_buildVersion;
	}
}

inline void StaticLightContainer::update()
{
	if (_manager != nullptr && _manager->_buildVersion != _buildVersion)
		build();
}
