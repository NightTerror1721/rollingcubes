#include "entities.h"

#include "utils/shader_constants.h"


ModeledEntity::~ModeledEntity()
{
	_staticLightManager.reset();
}

void ModeledEntity::update(Time elapsedTime)
{
	if (isAlteredFlagEnabled())
	{
		if (_staticLightManager != nullptr)
			_staticLightContainer.setPosition(getPosition());
	}

	_staticLightContainer.update();
	disableAlteredFlag();
}

void ModeledEntity::renderDefault(GLenum mode)
{
    auto shader = getLightningShader();
	if (shader == nullptr || _objModel == nullptr)
		return;

	shader->use();
	_material.bindTextures();

	shader[constants::uniform::model_data::model()] = getModelMatrix();
	shader[constants::uniform::model_data::modelNormal()] = getNormalMatrix();
	if (_staticLightManager != nullptr)
		shader->setUniformStaticLights(_staticLightContainer);
	shader->setUniformMaterial(_material);

	_objModel->render(mode);

	_material.unbindTextures();
	shader->notUse();
}

void ModeledEntity::linkStaticLightManager(const std::shared_ptr<StaticLightManager>& lightManager)
{
	_staticLightManager = lightManager;
	_staticLightContainer.link(lightManager);
}
