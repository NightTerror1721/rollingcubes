#include "skybox.h"

#include "utils/shader_constants.h"
#include "cube_model.h"

#include "engine/lua/module.h"
#include "utils/lualib_constants.h"


SkyboxTemplateManager SkyboxTemplateManager::Instance;


Skybox::Skybox() : TransformableEntity(), LocalValuesContainer()
{
	_shader = ShaderProgramManager::instance().getSkyShaderProgram();
}

void Skybox::update(Time elapsedTime)
{
    if (_template != nullptr)
        _template->onUpdate(*this, elapsedTime);
}

void Skybox::render(const Camera& cam)
{
    if (_template != nullptr)
        _template->onRender(*this, cam);

	if (_shader != nullptr && _texture != nullptr)
	{
		auto model = getDefaultModel();
        glm::mat4 view = cam.getCenteredViewMatrix();

        glDepthFunc(GL_LEQUAL);
		_shader->use();
		_texture->activate(0);
		_shader["skybox"] = 0;
        _shader["view"] = view;
        _shader["projection"] = cam.getProjectionMatrix();
		model->render();
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












namespace lua::lib
{
	namespace LUA_skyboxes { static defineLuaLibraryConstructor(registerToLua, root, state); }

	void registerSkyboxessLibToLua()
	{
		LuaLibraryManager::instance().registerLibrary(
			::lua::lib::names::skyboxes,
			&LUA_skyboxes::registerToLua,
			{ ::lua::lib::names::themes, ::lua::lib::names::models }
		);
	}
}

namespace lua::lib::LUA_skyboxes
{
    static defineLuaLibraryConstructor(registerToLua, root, state)
	{
		namespace meta = lua::metamethod;


		auto clss = root.deriveClass<Skybox, TransformableEntity>("Skybox");
		clss
			// Fields //
			// Methods //
			;

		lua::lib::addLocalValuesContainerToClass(clss);

		root = clss.endClass();
		return true;
	}
}
