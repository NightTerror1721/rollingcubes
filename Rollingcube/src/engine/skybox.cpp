#include "skybox.h"

#include "utils/shader_constants.h"
#include "game/cube_model.h"


Skybox::Skybox() : TransformableEntity()
{
	using namespace constants::shader;

	static std::once_flag prepareOnceFlag;
	std::call_once(prepareOnceFlag, []() {
		ShaderProgramManager::instance().load(internals::shaderFiles(sky));
	});

	_shader = ShaderProgramManager::instance().getSkyShaderProgram();
}

void Skybox::render(GLenum mode)
{
	if (_shader != nullptr && _texture != nullptr)
	{
		auto model = cubes::model::getModel();
		if (model != nullptr)
		{
			glDepthMask(GL_FALSE);
			_shader->use();
			_texture->activate(0);
			_shader["skybox"] = 0;
			model->render(mode);
			glDepthMask(GL_TRUE);
		}
	}
}

void Skybox::bindCameraToShader(const Camera& cam)
{
	if (_shader != nullptr)
	{
		_shader["view"] = cam.getViewMatrix();
		_shader["projection"] = cam.getProjectionMatrix();
	}
}
