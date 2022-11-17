#include "block.h"

#include "engine/lua/lua.h"
#include "engine/lua/constants.h"
#include "utils/lualib_constants.h"


BlockTemplateManager BlockTemplateManager::Instance = {};





const Transformable& BlockSide::getTransform() const
{
	if (_updateVersion != _parent->getCurrentChangeVersion())
	{
		const auto& mid = cubes::side::getDefaultMiddlePosition(getSideId());
		_transform.setPosition(_parent->getPosition() + mid);
		_transform.setRotation(_parent->getRotation() + cubes::side::getRotationAngles(getSideId()));
		_transform.setScale(_parent->getScale());

		_updateVersion = _parent->getCurrentChangeVersion();
	}

	return _transform;
}

void BlockSide::luaRender(const Camera& cam)
{
	auto model = getTemplate();
	if (model)
		model->onRenderSide(*this, cam);
}

void BlockSide::renderTile(const Camera& cam, const Tile& tile)
{
	Material defaultMaterial;

	ConstReference<StaticLightContainer> staticLights = _parent->hasStaticLightManagerLinked()
		? std::addressof(_parent->getStaticLightContainer())
		: nullptr;

	Tile::RenderData renderData{
		.camera = std::addressof(cam),
		.transform = _parent,
		.material = std::addressof(defaultMaterial),
		.staticLights = staticLights
	};

	tile.render(_sideId, renderData);
}







Block::Block(Id id) :
	_blockId(id),
	_sides{
		Side(*this, cubes::side::ids[0]),
		Side(*this, cubes::side::ids[1]),
		Side(*this, cubes::side::ids[2]),
		Side(*this, cubes::side::ids[3]),
		Side(*this, cubes::side::ids[4]),
		Side(*this, cubes::side::ids[5])
	}
{

}

Block::~Block()
{

}

void Block::init()
{
	if (_template)
		_template->onBlockConstruct(*this);

	for (int i = 0; i < _sides.size(); ++i)
		_sides[i].init();
}

void Block::luaRender(const Camera& cam)
{
	if (_template)
		_template->onRender(*this, cam);

	for (int i = 0; i < _sides.size(); ++i)
		_sides[i].render(cam);
}

void Block::defaultRender(const Camera& cam)
{
	ModelableEntity::render(cam);
}

void Block::update(Time elapsedTime)
{
	if (_template)
		_template->onUpdate(*this, elapsedTime);

	for (int i = 0; i < _sides.size(); ++i)
		_sides[i].update(elapsedTime);

	ModelableEntity::update(elapsedTime);
}









////////////










namespace lua::lib
{
	namespace LUA_blocks { static defineLuaLibraryConstructor(registerToLua, root, state); }

	void registerBlocksLibToLua()
	{
		LuaLibraryManager::instance().registerLibrary(
			::lua::lib::names::blocks,
			&LUA_blocks::registerToLua,
			{::lua::lib::names::themes }
		);
	}
}

namespace lua::lib::LUA_blocks
{
	namespace LUA_blockSide
	{
		static int getSideId(const BlockSide* self) { return cubes::side::idToInt(self->getSideId()); }

		static BlockSide::Id getId(const BlockSide* self) { return self->getId(); }

		static const Block* getParent(const BlockSide* self) { return std::addressof(self->getParent()); }

		static const glm::vec3& getPosition(const BlockSide* self) { return self->getPosition(); }

		static const glm::vec3& getRotationAngles(const BlockSide* self) { return self->getRotationAngles(); }

		static const glm::vec3& getScale(const BlockSide* self) { return self->getScale(); }

		static const glm::vec3& getNormal(const BlockSide* self) { return self->getNormal(); }

		static const glm::mat4& getModelMatrix(const BlockSide* self) { return self->getModelMatrix(); }


		static void renderTile(BlockSide* self, const Camera* cam, const Tile* tile) { self->renderTile(*cam, *tile); }


		static defineLuaLibraryConstructor(registerToLua, root, state)
		{
			namespace meta = lua::metamethod;

			auto clss = root.beginClass<BlockSide>("BlockSide");
			clss
				// Fields //
				.addProperty("sideId", &getSideId)
				.addProperty("id", &getId)
				.addProperty("parent", &getParent)
				.addProperty("position", &getPosition)
				.addProperty("rotationAngles", &getRotationAngles)
				.addProperty("scale", &getScale)
				.addProperty("normal", &getNormal)
				.addProperty("modelMatrix", &getModelMatrix)
				// Methods //
				.addFunction("renderTile", &renderTile)
				;

			lua::lib::utils::addLuaLocalVariablesToClass(clss);

			root = clss.endClass();
			return true;
		}
	}

	namespace LUA_block
	{
		static Block::Id getBlockId(const Block* self) { return self->getBlockId(); }


		static Block::Side* getSide(Block* self, unsigned int sideId) { return std::addressof(self->getSide(cubes::side::intToId(sideId))); }

		static void render(Block* self, Camera* cam) { self->defaultRender(*cam); }


		static defineLuaLibraryConstructor(registerToLua, root, state)
		{
			namespace meta = lua::metamethod;

			auto clss = root.deriveClass<Block, ModelableEntity>("Block");
			clss
				// Fields //
				.addProperty("blockId", &getBlockId)
				// Methods //
				.addFunction("getSide", &getSide)
				.addFunction("render", &render)
				;

			lua::lib::utils::addLuaLocalVariablesToClass(clss);

			root = clss.endClass();
			return true;
		}
	}



	static defineLuaLibraryConstructor(registerToLua, root, state)
	{
		if (!LUA_blockSide::registerToLua(root, state))
			return false;

		if (!LUA_block::registerToLua(root, state))
			return false;

		return true;
	}
}
