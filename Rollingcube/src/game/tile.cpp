#include "tile.h"

#include "engine/lua/module.h"
#include "utils/lualib_constants.h"


TileTemplateManager TileTemplateManager::Instance = {};












namespace lua::lib
{
	namespace LUA_tile { static defineLuaLibraryConstructor(registerToLua, root, state); }

	void registerTilesLibToLua()
	{
		LuaLibraryManager::instance().registerLibrary(
			::lua::lib::names::tiles,
			&LUA_tile::registerToLua,
			{ ::lua::lib::names::themes }
		);
	}
}

namespace lua::lib::LUA_tile
{
	static const Camera* renderDataGetCamera(const TileRenderData* self) { return &self->camera; }

	static const Transformable* renderDataGetTransform(const TileRenderData* self) { return &self->transform; }

	static const Material* renderDataGetMaterial(const TileRenderData* self) { return &self->material; }
	static void renderDataSetMaterial(TileRenderData* self, const Material* value) { self->material = value; }


	static void tileRenderQuad(Tile* self, int sideId, const TileRenderData* renderData)
	{
		self->renderQuad(cubes::side::intToId(sideId), *renderData);
	}


	static defineLuaLibraryConstructor(registerToLua, root, state)
	{
		namespace meta = lua::metamethod;

		// Tile::RenderData //
		auto cls1 = root.beginClass<TileRenderData>("TileRenderData");
		cls1
			// Fields //
			.addProperty("camera", &renderDataGetCamera)
			.addProperty("transform", &renderDataGetTransform)
			.addProperty("material", &renderDataGetMaterial, &renderDataSetMaterial)
			;

		root = cls1.endClass();


		// Tile //
		auto cls2 = root.beginClass<Tile>("Tile");
		cls2
			// Fields //
			// Methods //
			.addFunction("renderQuad", &tileRenderQuad)
			;

		root = cls2.endClass();
		return true;
	}
}
