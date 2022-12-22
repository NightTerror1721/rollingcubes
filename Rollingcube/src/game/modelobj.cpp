#include "modelobj.h"

#include "engine/lua/module.h"
#include "utils/lualib_constants.h"


ModelObjectTemplateManager ModelObjectTemplateManager::Instance = {};












namespace lua::lib
{
	namespace LUA_modelobj { static defineLuaLibraryConstructor(registerToLua, root, state); }

	void registerModelsLibToLua()
	{
		LuaLibraryManager::instance().registerLibrary(
			::lua::lib::names::models,
			&LUA_modelobj::registerToLua,
			{ ::lua::lib::names::themes }
		);
	}
}

namespace lua::lib::LUA_modelobj
{
	static Camera* renderDataGetCamera(const ModelObjectRenderData* self) { return const_cast<Camera*>(&self->camera); }
	static void renderDataSetCamera(ModelObjectRenderData* self, Camera* value) { self->camera = value; }

	static const Transformable* renderDataGetTransform(const ModelObjectRenderData* self) { return &self->transform; }
	static void renderDataSetTransform(ModelObjectRenderData* self, Transformable* value) { self->transform = value; }

	static const Material* renderDataGetMaterial(const ModelObjectRenderData* self) { return &self->material; }
	static void renderDataSetMaterial(ModelObjectRenderData* self, Material* value) { self->material = value; }

	static const StaticLightContainer* renderDataGetStaticLights(const ModelObjectRenderData* self) { return &self->staticLights; }
	static void renderDataSetStaticLights(ModelObjectRenderData* self, StaticLightContainer* value) { self->staticLights = value; }

	static ModelObjectRenderData renderDataFromEntity(ModelableEntity* entity, Camera* cam)
	{
		return {
			.camera = cam,
			.transform = entity,
			.material = entity->getMaterial(),
			.staticLights = std::addressof(entity->getStaticLightContainer())
		};
	}


	static void modelObjectRenderModel(ModelObject* self, Model* model, ModelObjectRenderData* renderData) { self->renderModel(model, *renderData); }

	static void modelObjectRender(ModelObject* self, ModelObjectRenderData* renderData) { self->render(*renderData); }


	static const std::string& meshGetName(const Mesh* mesh) { return mesh->getName(); }

	static void meshRender(Mesh* self, ModelObjectRenderData* renderData) { ModelObject::renderMesh(*self, *renderData); }


	static defineLuaLibraryConstructor(registerToLua, root, state)
	{
		namespace meta = lua::metamethod;

		// ModelObject::RenderData //
		auto cls1 = root.beginClass<ModelObjectRenderData>("ModelObjectRenderData");
		cls1.addConstructor<void (*)()>()
			// Fields //
			.addProperty("camera", &renderDataGetCamera, &renderDataSetCamera)
			.addProperty("transform", &renderDataGetTransform, &renderDataSetTransform)
			.addProperty("material", &renderDataGetMaterial, &renderDataSetMaterial)
			.addProperty("staticLights", &renderDataGetStaticLights, &renderDataSetStaticLights)
			// Static Functions //
			.addStaticFunction("fromEntity", &renderDataFromEntity)
			;

		root = cls1.endClass();


		// Model //
		auto cls2 = root.beginClass<Model>("Model");
		cls2
			// Fields //
			// Methods //
			;
		root = cls2.endClass();


		// ModelObject //
		auto cls3 = root.beginClass<ModelObject>("ModelObject");
		cls3
			// Fields //
			// Methods //
			.addFunction("renderModel", &modelObjectRenderModel)
			.addFunction("render", &modelObjectRender)
			;
		root = cls3.endClass();


		// Mesh //
		auto cls4 = root.beginClass<Mesh>("Mesh");
		cls4
			// Fields //
			.addProperty("name", &meshGetName)
			// Methods //
			.addFunction("render", &meshRender)
			;

		root = cls4.endClass();
		return true;
	}
}
