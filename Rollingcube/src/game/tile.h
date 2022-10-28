#pragma once

#include "engine/entities.h"
#include "cube_model.h"
#include "luadefs.h"


namespace lua::lib { void registerTilesLibToLua(); }


class Tile;

struct TileRenderData
{
	ConstReference<Camera> camera;
	ConstReference<Transformable> transform;
	ConstReference<Material> material;
	ConstReference<StaticLightContainer> staticLights;
};

class TileModel : public LuaModel
{
public:
	using Ref = Reference<TileModel>;
	using ConstRef = ConstReference<TileModel>;

public:
	static constexpr std::string_view FunctionOnRender = "OnRender";

public:
	TileModel() = default;
	TileModel(const TileModel&) = delete;
	TileModel(TileModel&&) noexcept = default;
	~TileModel() = default;

	TileModel& operator= (const TileModel&) = delete;
	TileModel& operator= (TileModel&&) noexcept = default;

	inline Type getType() const override { return Type::Tile; }

public:
	void onRender(Tile& tile, cubes::side::Id sideId, TileRenderData& renderData);
};




class TileModelManager : public LuaModelManager<TileModel>
{
private:
	static TileModelManager Instance;

public:
	static inline TileModelManager& instance() { return Instance; }

private:
	inline TileModelManager() : LuaModelManager() {}
};




class Tile
{
public:
	using RenderData = TileRenderData;

private:
	TileModel::Ref _model = nullptr;

public:
	Tile() = default;
	Tile(const Tile&) = default;
	Tile(Tile&&) noexcept = default;
	~Tile() = default;

	Tile& operator= (const Tile&) = default;
	Tile& operator= (Tile&&) noexcept = default;

public:
	constexpr Tile(TileModel::Ref model) : _model(model) {}

	inline void render(cubes::side::Id sideId, RenderData& renderData) const
	{
		if (_model != nullptr)
			_model->onRender(const_cast<Tile&>(*this), sideId, renderData);
	}

	inline void renderQuad(cubes::side::Id sideId, const RenderData& renderData) const
	{
		if (_model != nullptr)
		{
			ModelableEntity::bindLightnigShaderRenderData(*renderData.camera, *renderData.transform, renderData.material, renderData.staticLights);
			cubes::model::render(sideId);
			ModelableEntity::unbindLightnigShaderRenderData(renderData.material);
		}
	}
};











inline void TileModel::onRender(Tile& tile, cubes::side::Id sideId, TileRenderData& renderData)
{
	vcall(FunctionOnRender, std::addressof(tile), cubes::side::idToInt(sideId), std::addressof(renderData));
}
