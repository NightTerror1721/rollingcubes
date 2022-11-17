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

class TileTemplate : public LuaTemplate
{
public:
	using Ref = Reference<TileTemplate>;
	using ConstRef = ConstReference<TileTemplate>;

public:
	static constexpr std::string_view FunctionOnRender = "OnRender";

public:
	TileTemplate() = default;
	TileTemplate(const TileTemplate&) = delete;
	TileTemplate(TileTemplate&&) noexcept = default;
	~TileTemplate() = default;

	TileTemplate& operator= (const TileTemplate&) = delete;
	TileTemplate& operator= (TileTemplate&&) noexcept = default;

	inline Type getType() const override { return Type::Tile; }

public:
	void onRender(Tile& tile, cubes::side::Id sideId, TileRenderData& renderData);
};




class TileTemplateManager : public LuaTemplateManager<TileTemplate>
{
private:
	static TileTemplateManager Instance;

public:
	static inline TileTemplateManager& instance() { return Instance; }

private:
	inline TileTemplateManager() : LuaTemplateManager() {}
};




class Tile
{
public:
	using RenderData = TileRenderData;

private:
	TileTemplate::Ref _template = nullptr;

public:
	Tile() = default;
	Tile(const Tile&) = default;
	Tile(Tile&&) noexcept = default;
	~Tile() = default;

	Tile& operator= (const Tile&) = default;
	Tile& operator= (Tile&&) noexcept = default;

public:
	constexpr Tile(TileTemplate::Ref templ) : _template(templ) {}

	constexpr TileTemplate::Ref getTemplate() const { return _template; }

	inline void render(cubes::side::Id sideId, RenderData& renderData) const
	{
		if (_template != nullptr)
			_template->onRender(const_cast<Tile&>(*this), sideId, renderData);
	}

	inline void renderQuad(cubes::side::Id sideId, const RenderData& renderData) const
	{
		if (_template != nullptr)
		{
			ModelableEntity::bindLightnigShaderRenderData(*renderData.camera, *renderData.transform, renderData.material, renderData.staticLights);
			cubes::model::render(sideId);
			ModelableEntity::unbindLightnigShaderRenderData(renderData.material);
		}
	}
};











inline void TileTemplate::onRender(Tile& tile, cubes::side::Id sideId, TileRenderData& renderData)
{
	vcall(FunctionOnRender, std::addressof(tile), cubes::side::idToInt(sideId), std::addressof(renderData));
}
