#include "entities.h"

#include "utils/shader_constants.h"



void ModelableEntity::update(Time elapsedTime)
{
	if (isCurrentChangeVersionUpdated())
	{
		_updateBoundingVolume = true;

		if (hasStaticLightManagerLinked())
			getStaticLightContainer().setPosition(getPosition());
	}

	getStaticLightContainer().update();

	TransformableEntity::update(elapsedTime);
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

void ModelableEntity::bindLightnigShaderRenderData(const Camera& cam) const
{
	bindLightnigShaderRenderData(cam, *this, internalGetMaterial(), hasStaticLightManagerLinked() ? std::addressof(getStaticLightContainer()) : nullptr);

	/*auto shader = getLightningShader();
	if (shader == nullptr)
		return;

	Material::ConstRef material = internalGetMaterial();
	if (material == nullptr)
		return;

	shader->use();

	shader[constants::uniform::camera::viewProjection()] = cam.getViewprojectionMatrix();
	shader[constants::uniform::camera::viewPos()] = cam.getPosition();

	material->bindTextures();

	shader[constants::uniform::model_data::model()] = getModelMatrix();
	shader[constants::uniform::model_data::modelNormal()] = getNormalMatrix();
	if (_staticLightManager != nullptr)
		shader->setUniformStaticLights(_staticLightContainer);
	shader->setUniformMaterial(*&material);*/
}

void ModelableEntity::unbindLightnigShaderRenderData() const
{
	unbindLightnigShaderRenderData(internalGetMaterial());
	/*auto shader = getLightningShader();
	auto material = internalGetMaterial();
	if (shader != nullptr && material != nullptr)
	{
		material->unbindTextures();
		shader->notUse();
	}*/
}

void ModelableEntity::bindLightnigShaderRenderData(
	const Camera& cam,
	const Transformable& transform,
	Material::ConstRef material,
	ConstReference<StaticLightContainer> staticLightContainer
) {
	ShaderProgram::Ref shader = ShaderProgramManager::instance().getLightningShaderProgram();
	if (shader == nullptr)
		return;

	if (material == nullptr)
		return;

	shader->use();

	shader[constants::uniform::camera::viewProjection()] = cam.getViewprojectionMatrix();
	shader[constants::uniform::camera::viewPos()] = cam.getPosition();

	material->bindTextures();

	shader[constants::uniform::model_data::model()] = transform.getModelMatrix();
	shader[constants::uniform::model_data::modelNormal()] = transform.getNormalMatrix();
	if (staticLightContainer != nullptr)
		shader->setUniformStaticLights(*staticLightContainer);
	shader->setUniformMaterial(*&material);
}

void ModelableEntity::unbindLightnigShaderRenderData(Material::ConstRef material)
{
	auto shader = ShaderProgramManager::instance().getLightningShaderProgram();
	if (shader != nullptr && material != nullptr)
	{
		material->unbindTextures();
		shader->notUse();
	}
}
