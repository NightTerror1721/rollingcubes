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
		const auto& model = getDefaultModel();

        glDepthFunc(GL_LEQUAL);
		_shader->use();
		_texture->activate(0);
		_shader["skybox"] = 0;
		model.render(mode);
        glDepthFunc(GL_LESS);
	}
}

void Skybox::bindCameraToShader(const Camera& cam)
{
	if (_shader != nullptr)
	{
        glm::mat4 view = glm::mat4(glm::mat3(cam.getViewMatrix()));

		_shader->use();
		_shader["view"] = view;
		_shader["projection"] = cam.getProjectionMatrix();
		_shader->notUse();
	}
}

std::unique_ptr<ObjModel> Skybox::DefaultModel = nullptr;

namespace raw_geom
{
	static constexpr glm::vec3 skyboxVertices[36]
	{
        { -1.0f,  1.0f, -1.0f },
        { -1.0f, -1.0f, -1.0f },
        {  1.0f, -1.0f, -1.0f },
        {  1.0f, -1.0f, -1.0f },
        {  1.0f,  1.0f, -1.0f },
        { -1.0f,  1.0f, -1.0f },

        { -1.0f, -1.0f,  1.0f },
        { -1.0f, -1.0f, -1.0f },
        { -1.0f,  1.0f, -1.0f },
        { -1.0f,  1.0f, -1.0f },
        { -1.0f,  1.0f,  1.0f },
        { -1.0f, -1.0f,  1.0f },

        {  1.0f, -1.0f, -1.0f },
        {  1.0f, -1.0f,  1.0f },
        {  1.0f,  1.0f,  1.0f },
        {  1.0f,  1.0f,  1.0f },
        {  1.0f,  1.0f, -1.0f },
        {  1.0f, -1.0f, -1.0f },

        { -1.0f, -1.0f,  1.0f },
        { -1.0f,  1.0f,  1.0f },
        {  1.0f,  1.0f,  1.0f },
        {  1.0f,  1.0f,  1.0f },
        {  1.0f, -1.0f,  1.0f },
        { -1.0f, -1.0f,  1.0f },

        { -1.0f,  1.0f, -1.0f },
        {  1.0f,  1.0f, -1.0f },
        {  1.0f,  1.0f,  1.0f },
        {  1.0f,  1.0f,  1.0f },
        { -1.0f,  1.0f,  1.0f },
        { -1.0f,  1.0f, -1.0f },

        { -1.0f, -1.0f, -1.0f },
        { -1.0f, -1.0f,  1.0f },
        {  1.0f, -1.0f, -1.0f },
        {  1.0f, -1.0f, -1.0f },
        { -1.0f, -1.0f,  1.0f },
        {  1.0f, -1.0f,  1.0f }
	};
}

void Skybox::loadDefaultModel()
{
    DefaultModel.reset(new ObjModel());

	auto omesh = DefaultModel->createMesh("default");
	if (!omesh)
		return;

	auto& mesh = *omesh;

	std::vector<glm::vec3> gl_vertices;
	gl_vertices.resize(sizeof(raw_geom::skyboxVertices) / sizeof(glm::vec3));
	std::memcpy(gl_vertices.data(), raw_geom::skyboxVertices, sizeof(raw_geom::skyboxVertices));

	mesh.setVertices(gl_vertices);
}


