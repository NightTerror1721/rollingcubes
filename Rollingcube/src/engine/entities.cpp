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
}

void ModelableEntity::unbindLightnigShaderRenderData() const
{
	unbindLightnigShaderRenderData(internalGetMaterial());
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

	shader->use();

	shader[constants::uniform::camera::viewProjection()] = cam.getViewprojectionMatrix();
	shader[constants::uniform::camera::viewPos()] = cam.getPosition();

	if (material != nullptr)
	{
		material->bindTextures();
		shader->setUniformMaterial(*&material);
	}

	shader[constants::uniform::model_data::model()] = transform.getModelMatrix();
	shader[constants::uniform::model_data::modelNormal()] = transform.getNormalMatrix();
	if (staticLightContainer != nullptr)
		shader->setUniformStaticLights(*staticLightContainer);
}

void ModelableEntity::unbindLightnigShaderRenderData(Material::ConstRef material)
{
	auto shader = ShaderProgramManager::instance().getLightningShaderProgram();
	if (shader != nullptr)
		shader->notUse();
	if(material != nullptr)
		material->unbindTextures();
}
