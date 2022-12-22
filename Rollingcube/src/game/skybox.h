#pragma once

#include "engine/entities.h"
#include "engine/texture.h"
#include "engine/camera.h"

#include "luadefs.h"


namespace lua::lib { void registerSkyboxessLibToLua(); }


class Skybox;

class SkyboxTemplate : public LuaTemplate
{
public:
	using Ref = Reference<SkyboxTemplate>;
	using ConstRef = ConstReference<SkyboxTemplate>;

public:
	static constexpr std::string_view FunctionOnConstruct = "OnConstruct";
	static constexpr std::string_view FunctionOnRender = "OnRender";
	static constexpr std::string_view FunctionOnUpdate = "OnUpdate";

public:
	SkyboxTemplate() = default;
	SkyboxTemplate(const SkyboxTemplate&) = delete;
	SkyboxTemplate(SkyboxTemplate&&) noexcept = default;
	~SkyboxTemplate() = default;

	SkyboxTemplate& operator= (const SkyboxTemplate&) = delete;
	SkyboxTemplate& operator= (SkyboxTemplate&&) noexcept = default;

	inline Type getType() const override { return Type::Skybox; }

public:
	inline void onConstruct(Skybox& skybox) { vcall(FunctionOnConstruct, std::addressof(skybox)); }
	inline void onRender(Skybox& skybox, const Camera& cam) { vcall(FunctionOnRender, std::addressof(skybox), std::addressof(const_cast<Camera&>(cam))); }
	inline void onUpdate(Skybox& skybox, Time elapsedTime) { vcall(FunctionOnUpdate, std::addressof(skybox), double(elapsedTime.toSeconds())); }
};


class SkyboxTemplateManager : public LuaTemplateManager<SkyboxTemplate>
{
private:
	static SkyboxTemplateManager Instance;

public:
	static inline SkyboxTemplateManager& instance() { return Instance; }

private:
	inline SkyboxTemplateManager() : LuaTemplateManager() {}
};


class Skybox : public TransformableEntity, public LocalValuesContainer
{
private:
	SkyboxTemplate::Ref _template = nullptr;
	CubeMapTexture::Ref _texture = nullptr;
	ShaderProgram::Ref _shader = nullptr;

public:
	Skybox();
	Skybox(const Skybox&) = default;
	Skybox(Skybox&&) noexcept = default;
	~Skybox() = default;

	Skybox& operator= (const Skybox&) = default;
	Skybox& operator= (Skybox&&) noexcept = default;

public:
	void update(Time elapsedTime) override;
	void render(const Camera& cam) override;

public:
	constexpr void setTemplate(SkyboxTemplate::Ref templ) { _template = templ; }
	constexpr SkyboxTemplate::Ref getTemplate() const { return _template; }

	constexpr void setTexture(CubeMapTexture::Ref texture) { _texture = texture; }
	constexpr CubeMapTexture::Ref getTexture() const { return _texture; }

	constexpr void setShader(ShaderProgram::Ref shader) { _shader = shader; }
	constexpr ShaderProgram::Ref getShader() const { return _shader; }

private:
	static std::unique_ptr<Model> DefaultModel;

	static void loadDefaultModel();

public:
	static inline Model::Ref getDefaultModel()
	{
		if (DefaultModel == nullptr)
			loadDefaultModel();
		return DefaultModel.get();
	}
};

