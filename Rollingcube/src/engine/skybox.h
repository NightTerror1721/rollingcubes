#pragma once

#include "entities.h"
#include "texture.h"
#include "camera.h"

class Skybox : public TransformableEntity
{
private:
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
	void render(const Camera& cam) override;

public:
	constexpr void setTexture(CubeMapTexture::Ref texture) { _texture = texture; }
	constexpr CubeMapTexture::Ref getTexture() const { return _texture; }

	constexpr void setShader(ShaderProgram::Ref shader) { _shader = shader; }
	constexpr ShaderProgram::Ref getShader() const { return _shader; }

private:
	static std::unique_ptr<Model> DefaultModel;

	static void loadDefaultModel();

	static inline const Model& getDefaultModel()
	{
		if (DefaultModel == nullptr)
			loadDefaultModel();
		return *DefaultModel;
	}
};

