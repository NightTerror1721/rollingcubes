#pragma once

#include <array>

#include "engine/entities.h"

#include "cube_model.h"
#include "luadefs.h"
#include "tile.h"


namespace lua::lib { void registerBlocksLibToLua(); }



class Block;
class BlockSide;

class BlockModel : public LuaModel
{
public:
	using Ref = Reference<BlockModel>;
	using ConstRef = ConstReference<BlockModel>;

public:
	static constexpr std::string_view FunctionOnRender = "OnRender";
	static constexpr std::string_view FunctionOnRenderSide = "OnRenderSide";
	static constexpr std::string_view FunctionOnUpdate = "OnUpdate";
	static constexpr std::string_view FunctionOnUpdateSide = "OnUpdateSide";

	static constexpr std::string_view FunctionOnBlockConstruct = "OnBlockConstruct";
	static constexpr std::string_view FunctionOnBlockSideConstruct = "OnBlockSideConstruct";

public:
	BlockModel() = default;
	BlockModel(const BlockModel&) = delete;
	BlockModel(BlockModel&&) noexcept = default;
	~BlockModel() = default;

	BlockModel& operator= (const BlockModel&) = delete;
	BlockModel& operator= (BlockModel&&) noexcept = default;

	inline Type getType() const override { return Type::Block; }

public:
	void onRender(Block& block, const Camera& cam);
	void onRenderSide(BlockSide& side, const Camera& cam);
	void onUpdate(Block& block, Time elapsedTime);
	void onUpdateSide(BlockSide& side, Time elapsedTime);

	void onBlockConstruct(Block& block);
	void onBlockSideConstruct(BlockSide& side);
};




class BlockModelManager : public LuaModelManager<BlockModel>
{
private:
	static BlockModelManager Instance;

public:
	static inline BlockModelManager& instance() { return Instance; }

private:
	inline BlockModelManager() : LuaModelManager() {}
};





class BlockSide : public LuaLocalVariablesContainer, public Renderable, public Updatable
{
public:
	friend Block;

public:
	using SideId = cubes::side::Id;
	using Id = unsigned int;

private:
	SideId _sideId;
	Block* _parent;
	BlockModel::Ref _blockModel = nullptr;

	mutable VersionFlag _updateVersion = {};
	mutable Transformable _transform = {};

public:
	~BlockSide() = default;

	BlockSide(const BlockSide&) = delete;
	BlockSide(BlockSide&&) noexcept = default;

	BlockSide& operator= (const BlockSide&) = delete;
	BlockSide& operator= (BlockSide&&) noexcept = default;

	inline bool operator== (const BlockSide& right) const noexcept { return getId() == right.getId(); };

public:
	constexpr SideId getSideId() const { return _sideId; }

	constexpr Block& getParent() { return *_parent; }
	constexpr const Block& getParent() const { return *_parent; }

	constexpr void setBlockModel(BlockModel::Ref model) { _blockModel = model; }
	BlockModel::Ref getBlockModel() const;

	Id getId() const;

public:
	const Transformable& getTransform() const;

	void luaRender(const Camera& cam);
	void renderTile(const Camera& cam, const Tile& tile);


private:
	inline BlockSide(Block& parent, SideId sideId) : _sideId(sideId), _parent(std::addressof(parent)) {}

	inline void init()
	{
		auto model = getBlockModel();
		if(model)
			model->onBlockSideConstruct(*this);
	}

public:
	inline void render(const Camera& cam) override
	{
		luaRender(cam);
	}

	inline void update(Time elapsedTime) override
	{
		auto model = getBlockModel();
		if (model)
			model->onUpdateSide(*this, elapsedTime);
	}

public:
	inline const glm::mat4& getModelMatrix() const { return getTransform().getModelMatrix(); }
	inline const glm::mat4& getInvertedModelMatrix() const { return getTransform().getInvertedModelMatrix(); }

	inline const glm::vec3& getPosition() const { return getTransform().getPosition(); }
	inline const glm::vec3& getRotationAngles() const { return getTransform().getRotation(); }
	inline const glm::vec3& getScale() const { return getTransform().getScale(); }

	inline const glm::vec3& getNormal() const { return cubes::side::getNormal(getSideId()); }
};




class Block : public ModelableEntity, public LuaLocalVariablesContainer
{
public:
	friend BlockSide;

public:
	using Id = unsigned int;
	using Side = BlockSide;

private:
	Id _blockId;
	std::array<Side, cubes::side::count> _sides;
	BlockModel::Ref _blockModel;

public:
	Block(const Block&) = delete;
	Block(Block&&) noexcept = default;

	Block& operator= (const Block&) = delete;
	Block& operator= (Block&&) noexcept = default;

public:
	Block(Id id = 0);
	virtual ~Block();

	void init();

	void luaRender(const Camera& cam);
	void defaultRender(const Camera& cam);

	void update(Time elapsedTime) override;

public:
	constexpr void setBlockId(Id blockId) { _blockId = blockId; }
	constexpr Id getBlockId() const { return _blockId; }

	constexpr Side& getSide(Side::SideId sideId) { return _sides[cubes::side::idToInt(sideId)]; }
	constexpr const Side& getSide(Side::SideId sideId) const { return _sides[cubes::side::idToInt(sideId)]; }

	constexpr void setBlockModel(BlockModel::Ref model) { _blockModel = model; }
	constexpr BlockModel::Ref getBlockModel() const { return _blockModel; }

	inline void render(const Camera& cam) override { luaRender(cam); }

protected:
	Model::Ref internalGetModel() const override { return cubes::model::getModel(); }

public:
	constexpr Side& operator[] (Side::SideId sideId) { return getSide(sideId); }
	constexpr const Side& operator[] (Side::SideId sideId) const { return getSide(sideId); }
};


inline BlockSide::Id BlockSide::getId() const { return cubes::side::idToInt(_sideId) * _parent->getBlockId(); }









inline BlockModel::Ref BlockSide::getBlockModel() const { return _blockModel ? _blockModel : _parent->_blockModel; }


inline void BlockModel::onRender(Block& block, const Camera& cam) { vcall(FunctionOnRender, std::addressof(block), std::addressof(cam)); }
inline void BlockModel::onRenderSide(BlockSide& side, const Camera& cam) { vcall(FunctionOnRenderSide, std::addressof(side), std::addressof(cam)); }
inline void BlockModel::onUpdate(Block& block, Time elapsedTime) { vcall(FunctionOnUpdate, std::addressof(block), elapsedTime.toSeconds()); }
inline void BlockModel::onUpdateSide(BlockSide& side, Time elapsedTime) { vcall(FunctionOnUpdateSide, std::addressof(side), elapsedTime.toSeconds()); }

inline void BlockModel::onBlockConstruct(Block& block) { vcall(FunctionOnBlockConstruct, std::addressof(block)); }
inline void BlockModel::onBlockSideConstruct(BlockSide& side) { vcall(FunctionOnBlockSideConstruct, std::addressof(side)); }
