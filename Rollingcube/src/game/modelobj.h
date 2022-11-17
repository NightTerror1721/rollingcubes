#pragma once

#include "engine/entities.h"

#include "luadefs.h"


namespace lua::lib { void registerModelsLibToLua(); }


class ModelObject;

struct ModelObjectRenderData
{
	ConstReference<Camera> camera;
	ConstReference<Transformable> transform;
	ConstReference<Material> material;
	ConstReference<StaticLightContainer> staticLights;
};

class ModelObjectTemplate : public LuaTemplate
{
public:
	using Ref = Reference<ModelObjectTemplate>;
	using ConstRef = ConstReference<ModelObjectTemplate>;

public:
	static constexpr std::string_view FunctionOnRender = "OnRender";
	static constexpr std::string_view FunctionOnRenderMesh = "OnRenderMesh";

public:
	ModelObjectTemplate() = default;
	ModelObjectTemplate(const ModelObjectTemplate&) = delete;
	ModelObjectTemplate(ModelObjectTemplate&&) noexcept = default;
	~ModelObjectTemplate() = default;

	ModelObjectTemplate& operator= (const ModelObjectTemplate&) = delete;
	ModelObjectTemplate& operator= (ModelObjectTemplate&&) noexcept = default;

	inline Type getType() const override { return Type::Model; }

public:
	inline void onRender(ModelObject& model, ModelObjectRenderData& renderData)
	{
		vcall(FunctionOnRender, std::addressof(model), std::addressof(renderData));
	}

	inline void onRenderMesh(Mesh& mesh, ModelObjectRenderData& renderData)
	{
		vcall(FunctionOnRenderMesh, std::addressof(mesh), std::addressof(renderData));
	}
};




class ModelObject
{
public:
	using RenderData = ModelObjectRenderData;

private:
	ModelObjectTemplate::Ref _template = nullptr;

public:
	ModelObject() = default;
	ModelObject(const ModelObject&) = default;
	ModelObject(ModelObject&&) noexcept = default;
	~ModelObject() = default;

	ModelObject& operator= (const ModelObject&) = default;
	ModelObject& operator= (ModelObject&&) noexcept = default;

public:
	constexpr ModelObject(ModelObjectTemplate::Ref templ) : _template(templ) {}

	constexpr ModelObjectTemplate::Ref getTemplate() const { return _template; }

	inline void render(RenderData& renderData) const
	{
		if (_template != nullptr)
			_template->onRender(const_cast<ModelObject&>(*this), renderData);
	}

	inline void renderModel(Model::Ref model, RenderData& renderData) const
	{
		if (model != nullptr)
		{
			ModelableEntity::bindLightnigShaderRenderData(*renderData.camera, *renderData.transform, nullptr, renderData.staticLights);
			for (const auto& mesh : *model)
				_template->onRenderMesh(const_cast<Mesh&>(*mesh), renderData);
			ModelableEntity::unbindLightnigShaderRenderData(nullptr);
		}
	}

public:
	static inline void renderMesh(const Mesh& mesh, const RenderData& renderData)
	{
		if(renderData.material != nullptr)
			renderData.material->bindTextures();

		mesh.render();

		if (renderData.material != nullptr)
			renderData.material->unbindTextures();
	}
};




class ModelObjectTemplateManager : public LuaTemplateManager<ModelObjectTemplate>
{
private:
	static ModelObjectTemplateManager Instance;

public:
	static inline ModelObjectTemplateManager& instance() { return Instance; }

private:
	inline ModelObjectTemplateManager() : LuaTemplateManager() {}
};
