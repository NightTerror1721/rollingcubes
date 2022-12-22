#include "entities.h"

#include "lua/module.h"
#include "utils/lualib_constants.h"


namespace lua::lib
{
	namespace LUA_entities { static defineLuaLibraryConstructor(registerToLua, root, state); }

	void registerEntitiesLibToLua()
	{
		LuaLibraryManager::instance().registerLibrary(
			::lua::lib::names::entities,
			&LUA_entities::registerToLua,
			{ ::lua::lib::names::geometry, ::lua::lib::names::camera, ::lua::lib::names::shader });
	}
}

namespace lua::lib::LUA_entities
{
	namespace LUA_colorchannels
	{
		static const glm::vec3& getAmbientColor(const ColorChannels* self) { return self->getAmbientColor(); }
		static void setAmbientColor(ColorChannels* self, const glm::vec3& color) { self->setAmbientColor(color); }

		static const glm::vec3& getDiffuseColor(const ColorChannels* self) { return self->getDiffuseColor(); }
		static void setDiffuseColor(ColorChannels* self, const glm::vec3& color) { self->setDiffuseColor(color); }

		static const glm::vec3& getSpecularColor(const ColorChannels* self) { return self->getSpecularColor(); }
		static void setSpecularColor(ColorChannels* self, const glm::vec3& color) { self->setSpecularColor(color); }

		static void setColor(ColorChannels* self, const glm::vec3& color) { self->setColor(color); }

		static defineLuaLibraryConstructor(registerToLua, root, state)
		{
			namespace meta = ::lua::metamethod;

			auto clss = root.beginClass<ColorChannels>("ColorChannels");
			clss.addConstructor<void(*)()>()
				.addProperty("ambientColor", &getAmbientColor, &setAmbientColor)
				.addProperty("diffuseColor", &getDiffuseColor, &setDiffuseColor)
				.addProperty("specularColor", &getSpecularColor, &setSpecularColor)
				.addFunction("setColor", &setColor);

			root = clss.endClass();
			return true;
		}
	}

	namespace LUA_material
	{
		static Texture* getDiffuseTexture(const Material* self) { return &self->getDiffuseTexture(); }
		static void setDiffuseTexture(Material* self, Texture* texture) { self->setDiffuseTexture(texture); }

		static Texture* getSpecularTexture(const Material* self) { return &self->getSpecularTexture(); }
		static void setSpecularTexture(Material* self, Texture* texture) { self->setSpecularTexture(texture); }

		static Texture* getNormalsTexture(const Material* self) { return &self->getNormalsTexture(); }
		static void setNormalsTexture(Material* self, Texture* texture) { self->setNormalsTexture(texture); }

		static float getShininess(const Material* self) { return self->getShininess(); }
		static void setShininess(Material* self, float shininess) { self->setShininess(shininess); }

		static float getOpacity(const Material* self) { return self->getOpacity(); }
		static void setOpacity(Material* self, float shininess) { self->setOpacity(shininess); }

		static defineLuaLibraryConstructor(registerToLua, root, state)
		{
			namespace meta = ::lua::metamethod;

			auto clss = root.deriveClass<Material, ColorChannels>("Material");
			clss.addConstructor<void(*)()>()
				.addProperty("diffuseTexture", &getDiffuseTexture, &setDiffuseTexture)
				.addProperty("specularTexture", &getSpecularTexture, &setSpecularTexture)
				.addProperty("normalsTexture", &getNormalsTexture, &setNormalsTexture)
				.addProperty("shininess", &getShininess, &setShininess)
				.addProperty("opacity", &getOpacity, &setOpacity)
				.addFunction("hasTransparency", &Material::hasTransparency)
				.addFunction("bindTexturesToLightningShader", &Material::bindTextures)
				.addFunction("unbindTextures", &Material::unbindTextures);

			root = clss.endClass();
			return true;
		}
	}

	namespace LUA_texture
	{
		namespace filters::mag
		{
			using type = GLint;
			static constinit type Nearest = GLint(Texture::MagnificationFilter::Nearest);
			static constinit type Bilinear = GLint(Texture::MagnificationFilter::Bilinear);
		}

		namespace filters::min
		{
			using type = GLint;
			static constinit type Nearest = type(Texture::MinificationFilter::Nearest);
			static constinit type Bilinear = type(Texture::MinificationFilter::Bilinear);
			static constinit type NearestMipmap = type(Texture::MinificationFilter::NearestMipmap);
			static constinit type BilinearMipmap = type(Texture::MinificationFilter::BilinearMipmap);
			static constinit type Trilinear = type(Texture::MinificationFilter::Trilinear);
		}

		namespace formats
		{
			using type = GLenum;
			static constinit type ColorIndex = type(Texture::Format::color_index);
			static constinit type Red = type(Texture::Format::red);
			static constinit type Green = type(Texture::Format::green);
			static constinit type Blue = type(Texture::Format::blue);
			static constinit type Alpha = type(Texture::Format::alpha);
			static constinit type RGB = type(Texture::Format::rgb);
			static constinit type RGBA = type(Texture::Format::rgba);
			static constinit type BGR = type(Texture::Format::bgr);
			static constinit type BGRA = type(Texture::Format::bgra);
			static constinit type Luminance = type(Texture::Format::luminance);
			static constinit type LuminanceAlpha = type(Texture::Format::luminance_alpha);
			static constinit type DepthComponent = type(Texture::Format::depth_component);
		}

		static Texture::Id getId(const Texture* self) { return self->getId(); }

		static Texture::SizeType getWidth(const Texture* self) { return self->getWidth(); }

		static Texture::SizeType getHeight(const Texture* self) { return self->getHeight(); }

		static std::string getFilePath(const Texture* self) { return self->getFilePath().data(); }


		static void setMagFilter(Texture* self, GLint filter) { self->setFilter(Texture::MagnificationFilter(filter)); }
		static void setMinFilter(Texture* self, GLint filter) { self->setFilter(Texture::MinificationFilter(filter)); }

		static bool create(Texture* self, Texture::SizeType width, Texture::SizeType height, GLenum format, LuaRef generateMipmapsLUA)
		{
			bool generateMipmaps = generateMipmapsLUA.isBool() ? generateMipmapsLUA.cast<bool>().value() : false;
			return self->create(width, height, Texture::Format(format), generateMipmaps);
		}

		static bool loadFromFile(Texture* self, const std::string& filepath, LuaRef generateMipmapsLUA)
		{
			bool generateMipmaps = generateMipmapsLUA.isBool() ? generateMipmapsLUA.cast<bool>().value() : true;
			return self->loadFromImage(filepath, generateMipmaps);
		}

		static bool resize(Texture* self, Texture::SizeType width, Texture::SizeType height, LuaRef generateMipmapsLUA)
		{
			bool generateMipmaps = generateMipmapsLUA.isBool() ? generateMipmapsLUA.cast<bool>().value() : false;
			return self->resize(width, height, generateMipmaps);
		}


		static defineLuaLibraryConstructor(registerToLua, root, state)
		{
			namespace meta = ::lua::metamethod;

			// Texture::MagnificationFilter //
			root = root.beginNamespace("TextureMagnificationFilter")
				.addVariable("Nearest", &filters::mag::Nearest)
				.addVariable("Bilinear", &filters::mag::Bilinear)
				.endNamespace();

			// Texture::MinificationFilter //
			root = root.beginNamespace("TextureMinificationFilter")
				.addVariable("Nearest", &filters::min::Nearest)
				.addVariable("Bilinear", &filters::min::Bilinear)
				.addVariable("NearestMipmap", &filters::min::NearestMipmap)
				.addVariable("BilinearMipmap", &filters::min::BilinearMipmap)
				.addVariable("Trilinear", &filters::min::Trilinear)
				.endNamespace();

			// Texture //
			auto clss = root.beginClass<Texture>("Texture");
			clss.addConstructor<void(*)()>()
				.addProperty("id", &getId)
				.addProperty("width", &getWidth)
				.addProperty("height", &getHeight)
				.addProperty("filePath", &getFilePath)
				.addFunction("isCreated", &Texture::isCreated)
				.addFunction("hasFile", &Texture::hasFile)
				.addFunction("bind", &Texture::bind)
				.addFunction("unbind", &Texture::unbind)
				.addFunction("activate", &Texture::activate)
				.addFunction("setMagnificationFilter", &setMagFilter)
				.addFunction("setMinificationFilter", &setMinFilter)
				.addFunction("setRepeat", &Texture::setRepeat)
				.addFunction("create", &create)
				.addFunction("loadFromFile", &loadFromFile)
				.addFunction("resize", &resize)
				.addFunction("destroy", &Texture::destroy);

			root = clss.endClass();
			return true;
		}
	}

	namespace LUA_cubemaptexture
	{
		static CubeMapTexture::Id getId(const CubeMapTexture* self) { return self->getId(); }

		static CubeMapTexture::SizeType getWidth(const CubeMapTexture* self) { return self->getWidth(); }

		static CubeMapTexture::SizeType getHeight(const CubeMapTexture* self) { return self->getHeight(); }

		static std::string getFilePath(const CubeMapTexture* self, int index) { return self->getFilePath(std::size_t(index)).data(); }


		static void setMagFilter(CubeMapTexture* self, GLint filter) { self->setFilter(CubeMapTexture::MagnificationFilter(filter)); }
		static void setMinFilter(CubeMapTexture* self, GLint filter) { self->setFilter(CubeMapTexture::MinificationFilter(filter)); }


		static defineLuaLibraryConstructor(registerToLua, root, state)
		{
			namespace meta = ::lua::metamethod;

			// CubeMapTexture //
			auto clss = root.beginClass<CubeMapTexture>("CubeMapTexture");
			clss.addConstructor<void(*)()>()
				.addProperty("id", &getId)
				.addProperty("width", &getWidth)
				.addProperty("height", &getHeight)
				.addFunction("getFilePath", &getFilePath)
				.addFunction("isCreated", &CubeMapTexture::isCreated)
				.addFunction("hasFile", &CubeMapTexture::hasFile)
				.addFunction("bind", &CubeMapTexture::bind)
				.addFunction("unbind", &CubeMapTexture::unbind)
				.addFunction("activate", &CubeMapTexture::activate)
				.addFunction("setMagnificationFilter", &setMagFilter)
				.addFunction("setMinificationFilter", &setMinFilter)
				.addFunction("setRepeat", &CubeMapTexture::setRepeat);

			root = clss.endClass();
			return true;
		}
	}

	namespace LUA_light
	{
		static const glm::vec3& getPosition(const Light* self) { return self->getPosition(); }
		static void setPosition(Light* self, const glm::vec3& position) { self->setPosition(position); }

		static float getConstantAttenuation(const Light* self) { return self->getConstantAttenuation(); }
		static void setConstantAttenuation(Light* self, float attenuation) { self->setConstantAttenuation(attenuation); }

		static float getLinearAttenuation(const Light* self) { return self->getLinearAttenuation(); }
		static void setLinearAttenuation(Light* self, float attenuation) { self->setLinearAttenuation(attenuation); }

		static float getQuadraticAttenuation(const Light* self) { return self->getQuadraticAttenuation(); }
		static void setQuadraticAttenuation(Light* self, float attenuation) { self->setQuadraticAttenuation(attenuation); }

		static float getIntensity(const Light* self) { return self->getIntensity(); }
		static void setIntensity(Light* self, float intensity) { self->setIntensity(intensity); }


		static defineLuaLibraryConstructor(registerToLua, root, state)
		{
			namespace meta = ::lua::metamethod;

			// Light //
			auto clss = root.deriveClass<Light, ColorChannels>("Light");
			clss.addConstructor<void(*)()>()
				.addProperty("position", &getPosition, &setPosition)
				.addProperty("constantAttenuation", &getConstantAttenuation, &setConstantAttenuation)
				.addProperty("linearAttenuation", &getLinearAttenuation, &setLinearAttenuation)
				.addProperty("quadraticAttenuation", &getQuadraticAttenuation, &setQuadraticAttenuation)
				.addProperty("intensity", &getIntensity, &setIntensity);

			root = clss.endClass();
			return true;
		}
	}

	namespace LUA_directionalLight
	{
		static const glm::vec3& getDirection(const DirectionalLight* self) { return self->getDirection(); }
		static void setDirection(DirectionalLight* self, const glm::vec3& position) { self->setDirection(position); }

		static float getIntensity(const DirectionalLight* self) { return self->getIntensity(); }
		static void setIntensity(DirectionalLight* self, float intensity) { self->setIntensity(intensity); }

		static void setDirectionFromAngles(DirectionalLight* self, float pitch, float yaw) { self->setDirectionFromAngles(pitch, yaw); }


		static defineLuaLibraryConstructor(registerToLua, root, state)
		{
			namespace meta = ::lua::metamethod;

			// Light //
			auto clss = root.deriveClass<DirectionalLight, ColorChannels>("DirectionalLight");
			clss.addConstructor<void(*)()>()
				.addProperty("direction", &getDirection, &setDirection)
				.addProperty("intensity", &getIntensity, &setIntensity)
				.addFunction("setDirectionFromAngles", &setDirectionFromAngles);

			root = clss.endClass();
			return true;
		}
	}

	namespace LUA_lightContainer
	{
		static const glm::vec3& getPosition(const StaticLightContainer* self) { return self->getPosition(); }

		static const Light* getLight(StaticLightContainer* self, lua_Integer index) { return std::addressof(self->at(std::size_t(index))); }


		static defineLuaLibraryConstructor(registerToLua, root, state)
		{
			namespace meta = ::lua::metamethod;

			// Light //
			auto clss = root.beginClass<StaticLightContainer>("StaticLightContainer");
			clss
				// Properties //
				.addProperty("position", &getPosition)
				// Operators //
				.addFunction(meta::len, &StaticLightContainer::size)
				.addFunction(meta::index, &getLight)
				// Methods //
				;

			root = clss.endClass();
			return true;
		}
	}

	namespace LUA_transformable
	{
		static const glm::vec3& getPosition(const Transformable* self) { return self->getPosition(); }
		static void setPosition(Transformable* self, const glm::vec3& value) { self->setPosition(value); }

		static const glm::vec3& getRotationAngles(const Transformable* self) { return self->getRotation(); }
		static void setRotationAngles(Transformable* self, const glm::vec3& value) { self->setRotation(value); }

		static const glm::vec3& getScale(const Transformable* self) { return self->getScale(); }
		static void setScale(Transformable* self, const glm::vec3& value) { self->setScale(value); }

		static glm::vec3 getRight(const Transformable* self) { return self->getRight(); }

		static glm::vec3 getUp(const Transformable* self) { return self->getUp(); }

		static glm::vec3 getForward(const Transformable* self) { return self->getForward(); }

		static glm::vec3 getFront(const Transformable* self) { return self->getFront(); }

		static glm::vec3 getBackward(const Transformable* self) { return self->getBackward(); }

		static glm::vec3 getGlobalPosition(const Transformable* self) { return self->getGlobalPosition(); }

		static glm::vec3 getGlobalScale(const Transformable* self) { return self->getGlobalScale(); }

		static const glm::mat4& getModelMatrix(const Transformable* self) { return self->getModelMatrix(); }

		static const glm::mat4& getNormalMatrix(const Transformable* self) { return self->getNormalMatrix(); }

		static const glm::mat4& getInvertedModelMatrix(const Transformable* self) { return self->getInvertedModelMatrix(); }


		static void move(Transformable* self, const glm::vec3& delta) { self->move(delta); }

		static void rotate(Transformable* self, const glm::vec3& delta) { self->rotate(delta); }

		static void scale(Transformable* self, const glm::vec3& delta) { self->scale(delta); }


		static defineLuaLibraryConstructor(registerToLua, root, state)
		{
			namespace meta = ::lua::metamethod;

			// Transformable //
			auto clss = root.beginClass<Transformable>("Transformable");
			clss.addConstructor<void(*)()>()
				// Fields //
				.addProperty("position", &getPosition, &setPosition)
				.addProperty("rotationAngles", &getRotationAngles, &setRotationAngles)
				.addProperty("scalation", &getScale, &setScale)
				.addProperty("basisRight", &getRight)
				.addProperty("basisForward", &getForward)
				.addProperty("basisFront", &getFront)
				.addProperty("basisBackward", &getBackward)
				.addProperty("globalPosition", &getGlobalPosition)
				.addProperty("globalScale", &getGlobalScale)
				.addProperty("modelMatrix", &getModelMatrix)
				.addProperty("normalMatrix", &getNormalMatrix)
				.addProperty("invertedModelMatrix", &getInvertedModelMatrix)
				// Methods //
				.addFunction("move", &move)
				.addFunction("rotate", &rotate)
				.addFunction("scale", &scale);

			root = clss.endClass();
			return true;
		}
	}

	namespace LUA_modelableEntity
	{
		static Entity::Id::IntegerType getEntityId(const ModelableEntity* self) { return self->getEntityId(); }

		static StaticLightContainer* getStaticLightContainer(const ModelableEntity* self)
		{
			return std::addressof(const_cast<StaticLightContainer&>(self->getStaticLightContainer()));
		}


		static bool operatorEq(const ModelableEntity* left, const ModelableEntity* right) { return (*left) == (*right); }


		static void render(ModelableEntity* self, const Camera* cam) { self->render(*cam); }

		static void renderWithLightningShader(ModelableEntity* self, const Camera* cam) { self->renderWithLightningShader(*cam); }

		static void renderBoundingVolume(ModelableEntity* self, const Camera* cam) { self->renderBoundingVolume(*cam); }

		static void update(ModelableEntity* self, double elapsedTime) { self->update(Time::seconds(elapsedTime)); }


		static defineLuaLibraryConstructor(registerToLua, root, state)
		{
			namespace meta = ::lua::metamethod;

			// Transformable //
			auto clss = root.deriveClass<ModelableEntity, Transformable>("Entity");
			clss
				// Fields //
				// Operators //
				.addFunction(meta::eq, &operatorEq)
				// Methods //
				.addFunction("render", &render)
				.addFunction("renderWithLightningShader", &renderWithLightningShader)
				.addFunction("renderBoundingVolume", &renderBoundingVolume)
				.addFunction("update", &update)
				.addFunction("hasTransparency", &ModelableEntity::hasTransparency)
				.addFunction("setForceTransparency", &ModelableEntity::setForceTransparency)
				.addFunction("isForceTransparencyEnabled", &ModelableEntity::isForceTransparencyEnabled)
				.addFunction("isVisibleInCamera", &ModelableEntity::isVisibleInCamera)
				;

			root = clss.endClass();
			return true;
		}
	}

	static defineLuaLibraryConstructor(registerToLua, root, state)
	{
		if (!LUA_colorchannels::registerToLua(root, state))
			return false;

		if (!LUA_material::registerToLua(root, state))
			return false;

		if (!LUA_texture::registerToLua(root, state))
			return false;

		if (!LUA_cubemaptexture::registerToLua(root, state))
			return false;

		if (!LUA_light::registerToLua(root, state))
			return false;

		if (!LUA_lightContainer::registerToLua(root, state))
			return false;

		if (!LUA_transformable::registerToLua(root, state))
			return false;

		if (!LUA_modelableEntity::registerToLua(root, state))
			return false;

		return true;
	}
}
