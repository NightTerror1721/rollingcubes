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

void Skybox::render(const Camera& cam)
{
	if (_shader != nullptr && _texture != nullptr)
	{
		const auto& model = getDefaultModel();
        glm::mat4 view = glm::mat4(glm::mat3(cam.getViewMatrix()));

        glDepthFunc(GL_LEQUAL);
		_shader->use();
		_texture->activate(0);
		_shader["skybox"] = 0;
        _shader["view"] = view;
        _shader["projection"] = cam.getProjectionMatrix();
		model.render();
        glDepthFunc(GL_LESS);
	}
}

std::unique_ptr<Model> Skybox::DefaultModel = nullptr;

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
    DefaultModel.reset(new Model());

	auto omesh = DefaultModel->createMesh("default");
	if (!omesh)
		return;

	auto& mesh = *omesh;

	std::vector<glm::vec3> gl_vertices;
	gl_vertices.resize(sizeof(raw_geom::skyboxVertices) / sizeof(glm::vec3));
	std::memcpy(gl_vertices.data(), raw_geom::skyboxVertices, sizeof(raw_geom::skyboxVertices));

	mesh.setVertices(gl_vertices);
}


