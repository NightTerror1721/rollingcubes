#include "block.h"

#include "core/parallel.h"
#include "engine/lua/lua.h"
#include "engine/lua/constants.h"
#include "utils/lualib_constants.h"

#include "theme.h"


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







Block::Block() :
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









void BlockContainer::clear()
{
	for (std::shared_ptr<Block> block = _first, next = nullptr; block != nullptr; block = next)
	{
		next = block->_nextBlock;
		block->_blockContainer = nullptr;
		block->_prevBlock.reset();
		block->_nextBlock.reset();
	}

	_first.reset();
	_last.reset();
	_net.clear();
	_allocator.clear();

	decltype(_unusedIds) newUnusedIds = {};
	std::swap(_unusedIds, newUnusedIds);
}

std::shared_ptr<Block> BlockContainer::createNewBlockFromTemplate(const std::string& templateName)
{
	auto blockTemplate = Theme::getCurrentTheme().getBlockTemplate(templateName);
	if (blockTemplate == nullptr)
	{
		logger::error("Block template {} not found.", templateName);
		return nullptr;
	}

	auto block = std::make_shared<Block>();
	block->setTemplate(blockTemplate);

	return block;
}

std::shared_ptr<Block> BlockContainer::createBlock(const Slot& slot, const std::string& templateName)
{
	auto block = createNewBlockFromTemplate(templateName);
	if (block == nullptr)
		return nullptr;

	if (!_net.setBlock(slot, block, true))
		return nullptr;

	if (_last == nullptr)
	{
		_first = block;
		_last = block;
	}
	else
	{
		block->_prevBlock = _last;
		_last->_nextBlock = block;
		_last = block;
	}

	block->_blockContainer = this;

	if (!_unusedIds.empty())
	{
		Block::Id id = _unusedIds.top();
		_unusedIds.pop();

		block->setBlockId(id);
		_allocator[bidToIdx(id)] = block;
	}
	else
	{
		Block::Id id = idxToBid(_allocator.size());
		block->setBlockId(id);
		_allocator.push_back(block);
	}

	block->init();
	return block;
}

bool BlockContainer::removeBlock(const Slot& slot)
{
	auto block = _net.eraseBlock(slot);
	if (block == nullptr)
		return false;

	if (block == _first)
	{
		if (_first == _last)
		{
			_first.reset();
			_last.reset();
		}
		else
		{
			_first->_nextBlock->_prevBlock.reset();
			_first = _first->_nextBlock;
		}
	}
	else if (block == _last)
	{
		if (_first == _last)
		{
			_first.reset();
			_last.reset();
		}
		else
		{
			_last->_prevBlock->_nextBlock.reset();
			_last = _last->_prevBlock;
		}
	}
	else
	{
		block->_prevBlock->_nextBlock = block->_nextBlock;
		block->_nextBlock->_prevBlock = block->_prevBlock;
	}

	block->_blockContainer = nullptr;
	block->_nextBlock.reset();
	block->_prevBlock.reset();

	Block::Id id = block->getBlockId();
	if (id != 0)
	{
		block->setBlockId(0);
		_unusedIds.push(id);
		_allocator[bidToIdx(id)].reset();
	}

	return true;
}

std::shared_ptr<Block> BlockContainer::getBlockById(Block::Id blockId) const
{
	if (blockId == 0)
		return nullptr;

	std::size_t idx = bidToIdx(blockId);
	if (idx < _allocator.size())
		return _allocator[idx];
	return nullptr;
}



Reference<Block::Side> BlockContainer::getBlockSideBySideId(Block::Side::Id sideId) const
{
	std::shared_ptr<Block> block = getBlockBySideId(sideId);
	if (block == nullptr)
		return nullptr;
	return std::addressof(block->getSide(Block::Side::SideId(sideId % cubes::side::count)));
}

void BlockContainer::render(const Camera& cam)
{
	const bool enabledTransparentList = _transparentRenderList != nullptr;
	for (std::shared_ptr<Block> block = _first; block != nullptr; block = block->_nextBlock)
	{
		if (block->isVisibleInCamera(cam))
		{
			if (!block->hasTransparency() || !enabledTransparentList)
				block->render(cam);
			else
				_transparentRenderList->addEntity(block, cam.getDistanceTo(block->getPosition()));
		}
	}
}

void BlockContainer::update(Time elapsedTime)
{
/*#ifdef PARALLEL_ENABLED
	std::shared_ptr<Block> currentBlock = _first;
	std::shared_ptr<Block> block = nullptr;
	bool end = false;

	#pragma omp parallel firstprivate(block, elapsedTime, end) shared(currentBlock)
	while (!end)
	{
		#pragma omp critical
		{
			if (currentBlock == nullptr)
				end;
			else
			{
				block = currentBlock;
				currentBlock = currentBlock->_nextBlock;
			}
		}

		if(!end)
			block->update(elapsedTime);
	}

#else*/
	for (std::shared_ptr<Block> block = _first; block != nullptr; block = block->_nextBlock)
		block->update(elapsedTime);
//#endif
}				









//////////////////////////////////////////////////////////////////////////////////////////////////










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
