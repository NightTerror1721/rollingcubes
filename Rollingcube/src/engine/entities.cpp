#include "entities.h"


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
    const auto& shader = Shader::getDefault();
	if (shader == nullptr || _objModel == nullptr)
		return;

	shader->bind();
	_material.bindTextures();

    shader->setUniformMatrix("model", getModelMatrix());
    shader->setUniformMatrix("modelNormal", getNormalMatrix());
	if (_staticLightManager != nullptr)
		shader->setUniformStaticLights(_staticLightContainer);
	shader->setUniformMaterial(_material);

	_objModel->render(mode);

	_material.unbindTextures();
	shader->unbind();
}

void ModeledEntity::linkStaticLightManager(const std::shared_ptr<StaticLightManager>& lightManager)
{
	_staticLightManager = lightManager;
	_staticLightContainer.link(lightManager);
}
