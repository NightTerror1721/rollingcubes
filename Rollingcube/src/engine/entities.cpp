#include "entities.h"

#include "utils/shader_constants.h"



ModelableEntity::~ModelableEntity()
{
	_staticLightManager.reset();
}

void ModelableEntity::update(Time elapsedTime)
{
	if (isAlteredFlagEnabled())
	{
		_updateBoundingVolume = true;

		if (_staticLightManager != nullptr)
			_staticLightContainer.setPosition(getPosition());
	}

	_staticLightContainer.update();
	disableAlteredFlag();
}

void ModelableEntity::renderWithLightningShader(const Camera& cam)
{
	auto model = internalGetModel();
	if (model != nullptr)
	{
		bindLightnigShaderRenderData(cam);
		model->render();
		unbindLightnigShaderRenderData();
	}
}

void ModelableEntity::linkStaticLightManager(const std::shared_ptr<StaticLightManager>& lightManager)
{
	_staticLightManager = lightManager;
	_staticLightContainer.link(lightManager);
}

void ModelableEntity::bindLightnigShaderRenderData(const Camera& cam) const
{
	auto shader = getLightningShader();
	if (shader == nullptr)
		return;

	shader->use();

	shader[constants::uniform::camera::viewProjection()] = cam.getViewprojectionMatrix();
	shader[constants::uniform::camera::viewPos()] = cam.getPosition();

	_material.bindTextures();

	shader[constants::uniform::model_data::model()] = getModelMatrix();
	shader[constants::uniform::model_data::modelNormal()] = getNormalMatrix();
	if (_staticLightManager != nullptr)
		shader->setUniformStaticLights(_staticLightContainer);
	shader->setUniformMaterial(_material);
}

void ModelableEntity::unbindLightnigShaderRenderData() const
{
	auto shader = getLightningShader();
	if (shader != nullptr)
	{
		_material.unbindTextures();
		shader->notUse();
	}
}
