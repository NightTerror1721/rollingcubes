#include "light.h"

#include <utility>


ShaderLights::~ShaderLights()
{
	if (_manager != nullptr)
		_manager->destroyShaderLights(_listenerId);
}

void ShaderLights::build()
{
	struct LightComparator
	{
		const LightManager::Allocator* alloc;
		float intensity;

		constexpr bool operator== (const LightComparator& right) const noexcept { return intensity == right.intensity; }
		constexpr auto operator<=> (const LightComparator& right) const noexcept { return intensity <=> right.intensity; }
	};

	if (_manager != nullptr)
	{
		for (Light::ManagerId id : _lights)
			_manager->releaseLightListener(id, _listenerId);

		_lights.clear();
		_lightPositions.clear();
		_lightDiffuseColors.clear();
		_lightAmbientColors.clear();
		_lightSpecularColors.clear();
		_lightConstants.clear();
		_lightLinears.clear();
		_lightQuadratics.clear();
		_lightIntensities.clear();

		std::vector<LightComparator> temp_lights;
		for (const auto& alloc : _manager->_lights)
		{
			float dist = glm::length(alloc.light->getPosition() - _position);
			float intensity = alloc.light->getIntensity() / (dist * dist);
			if (intensity >= minIntensity)
				temp_lights.push_back({ .alloc = std::addressof(alloc), .intensity = intensity });
		}

		if (!temp_lights.empty())
		{
			std::sort(temp_lights.begin(), temp_lights.end());
			if (temp_lights.size() > maxLights)
				temp_lights.resize(maxLights);

			const std::size_t len = temp_lights.size();
			_lights.resize(len);
			_lightPositions.resize(len);
			_lightDiffuseColors.resize(len);
			_lightAmbientColors.resize(len);
			_lightSpecularColors.resize(len);
			_lightConstants.resize(len);
			_lightLinears.resize(len);
			_lightQuadratics.resize(len);
			_lightIntensities.resize(len);

			for (std::size_t i = 0; i < len; ++i)
			{
				const Light* light = temp_lights[i].alloc->light.get();
				_lights[i] = temp_lights[i].alloc->id;
				_lightPositions[i] = light->getPosition();
				_lightDiffuseColors[i] = light->getDiffuseColor();
				_lightAmbientColors[i] = light->getAmbientColor();
				_lightSpecularColors[i] = light->getSpecularColor();
				_lightConstants[i] = light->getConstantAttenuation();
				_lightLinears[i] = light->getLinearAttenuation();
				_lightQuadratics[i] = light->getQuadraticAttenuation();
				_lightIntensities[i] = light->getIntensity();
			}
		}
	}
}



std::shared_ptr<ShaderLights> LightManager::createShaderLights()
{
	Light::ListenerId id = _listeners.createNew(std::make_shared<ShaderLights>(*this));
	std::shared_ptr<ShaderLights>& ptr = _listeners[id];
	ptr->_listenerId = id;
	return ptr;
}

void LightManager::destroyShaderLights(Light::ListenerId id)
{
	_listeners.destroy(id);
	for (auto& alloc : _lights)
		alloc.listeners.erase(id);
}

void LightManager::releaseLightListener(Light::ManagerId lightId, Light::ListenerId listenerId)
{
	if (managerIdToIndex(lightId) < _lights.size())
		_lights[lightId].listeners.erase(listenerId);
}

Light::ManagerId LightManager::createNewLight(const Light& initial_light_data)
{
	Light::ManagerId id = _lights.createNew();
	Allocator& alloc = _lights[id];
	alloc.light = std::make_shared<Light>(initial_light_data);
	alloc.id = id;

	return id;
}

void LightManager::updateLight(Light::ManagerId id, const Light& light)
{
	if (managerIdToIndex(id) < _lights.size())
	{
		Allocator& alloc = _lights[id];
		*alloc.light = light;
		
		for (auto e : alloc.listeners)
			_listeners[e]->_needBuild = true;
	}
}
