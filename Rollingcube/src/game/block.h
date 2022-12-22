#pragma once

#include <array>
#include <queue>

#include "engine/entities.h"

#include "cube_model.h"
#include "luadefs.h"
#include "tile.h"
#include "basics.h"


namespace lua::lib { void registerBlocksLibToLua(); }




enum class Orientation : int
{
	Right = 0,
	Front = 1,
	Left = 2,
	Back = 3,

	Default = Right
};

namespace utils
{
	constexpr Orientation normalize(Orientation o)
	{
		return Orientation(glm::utils::normalizeRange(int(o), int(Orientation::Right), int(Orientation::Back)));
	}
}




class Block;
class BlockSide;
class BlocksNet;
class BlockContainer;
class BlockContainerIterator;
class ConstBlockContainerIterator;

class BlockTemplate : public LuaTemplate
{
public:
	using Ref = Reference<BlockTemplate>;
	using ConstRef = ConstReference<BlockTemplate>;

public:
	static constexpr std::string_view FunctionOnRender = "OnRender";
	static constexpr std::string_view FunctionOnRenderSide = "OnRenderSide";
	static constexpr std::string_view FunctionOnUpdate = "OnUpdate";
	static constexpr std::string_view FunctionOnUpdateSide = "OnUpdateSide";

	static constexpr std::string_view FunctionOnBlockConstruct = "OnBlockConstruct";
	static constexpr std::string_view FunctionOnBlockSideConstruct = "OnBlockSideConstruct";

public:
	BlockTemplate() = default;
	BlockTemplate(const BlockTemplate&) = delete;
	BlockTemplate(BlockTemplate&&) noexcept = default;
	~BlockTemplate() = default;

	BlockTemplate& operator= (const BlockTemplate&) = delete;
	BlockTemplate& operator= (BlockTemplate&&) noexcept = default;

	inline Type getType() const override { return Type::Block; }

public:
	void onRender(Block& block, const Camera& cam);
	void onRenderSide(BlockSide& side, const Camera& cam);
	void onUpdate(Block& block, Time elapsedTime);
	void onUpdateSide(BlockSide& side, Time elapsedTime);

	void onBlockConstruct(Block& block);
	void onBlockSideConstruct(BlockSide& side);
};




class BlockTemplateManager : public LuaTemplateManager<BlockTemplate>
{
private:
	static BlockTemplateManager Instance;

public:
	static inline BlockTemplateManager& instance() { return Instance; }

private:
	inline BlockTemplateManager() : LuaTemplateManager() {}
};





class BlockSide : public LocalValuesContainer, public Renderable, public Updatable
{
public:
	friend Block;

public:
	using SideId = cubes::side::Id;
	using Id = unsigned int;

private:
	SideId _sideId;
	Block* _parent;
	BlockTemplate::Ref _template = nullptr;

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

	constexpr void setTemplate(BlockTemplate::Ref templ) { _template = templ; }
	BlockTemplate::Ref getTemplate() const;

	Id getId() const;

public:
	const Transformable& getTransform() const;

	void luaRender(const Camera& cam);
	void renderTile(const Camera& cam, const Tile& tile);


private:
	inline BlockSide(Block& parent, SideId sideId) : _sideId(sideId), _parent(std::addressof(parent)) {}

	inline void init()
	{
		auto model = getTemplate();
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
		auto model = getTemplate();
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




#pragma warning(push)
#pragma warning(disable: 26495)
union BlockSlot
{
public:
	using ValueType = int;

public:
	struct { ValueType x, y, z; };
	struct { ValueType column, row, depth; };

public:
	constexpr BlockSlot() noexcept : x(0), y(0), z(0) {}
	constexpr BlockSlot(const BlockSlot& r) noexcept = default;
	constexpr BlockSlot(BlockSlot&& r) noexcept = default;
	constexpr ~BlockSlot() = default;

	constexpr BlockSlot& operator= (const BlockSlot&) noexcept = default;
	constexpr BlockSlot& operator= (BlockSlot&&) noexcept = default;

	constexpr bool operator== (const BlockSlot& r) const noexcept { return x == r.x && y == r.y && z == r.z; };
	constexpr auto operator<=> (const BlockSlot& r) const noexcept
	{
		auto cmp = x <=> r.x;
		if (cmp == 0)
		{
			cmp = y <=> r.y;
			if (cmp == 0)
				return z <=> r.z;
		}
		return cmp;
	}

	constexpr BlockSlot(ValueType x, ValueType y, ValueType z) : x(x), y(y), z(z) {}

public:
	constexpr glm::vec3 toPosition() const
	{
		return {
			x * cubes::side::size,
			y * cubes::side::size,
			z * cubes::side::size
		};
	}

	static BlockSlot fromPosition(const glm::vec3& pos)
	{
		return {
			static_cast<ValueType>(pos.x / cubes::side::size),
			static_cast<ValueType>(pos.y / cubes::side::size),
			static_cast<ValueType>(pos.z / cubes::side::size)
		};
	}
};
#pragma warning(pop)




namespace std
{
	template<>
	struct hash<BlockSlot>
	{
		constexpr std::size_t operator()(const BlockSlot& slot) const
		{
			if constexpr (sizeof(std::size_t) < 8)
				return (std::size_t(slot.x & 0x3ff) << 20) | (std::size_t(slot.y & 0x3ff) << 10) | std::size_t(slot.z & 0x3ff);
			else
				return (std::size_t(slot.x & 0x1fffff) << 42) | (std::size_t(slot.y & 0x1fffff) << 21) | std::size_t(slot.z & 0x1fffff);
		}
	};
}




class Block : public ModelableEntity, public LocalValuesContainer
{
public:
	friend BlockSide;
	friend BlocksNet;
	friend BlockContainer;
	friend BlockContainerIterator;
	friend ConstBlockContainerIterator;

public:
	using Id = unsigned int;
	using Side = BlockSide;
	using Slot = BlockSlot;

private:
	Id _blockId = 0;
	std::array<Side, cubes::side::count> _sides;
	BlockTemplate::Ref _template = nullptr;

	Slot _slot;
	Reference<BlockContainer> _blockContainer = nullptr;
	std::shared_ptr<Block> _nextBlock = nullptr;
	std::shared_ptr<Block> _prevBlock = nullptr;

public:
	Block(const Block&) = delete;
	Block(Block&&) noexcept = default;

	Block& operator= (const Block&) = delete;
	Block& operator= (Block&&) noexcept = default;

public:
	Block();
	virtual ~Block();

	void init();

	void luaRender(const Camera& cam);
	void defaultRender(const Camera& cam);

	void update(Time elapsedTime) override;

public:
	constexpr Id getBlockId() const { return _blockId; }

	constexpr Side& getSide(Side::SideId sideId) { return _sides[cubes::side::idToInt(sideId)]; }
	constexpr const Side& getSide(Side::SideId sideId) const { return _sides[cubes::side::idToInt(sideId)]; }

	constexpr void setTemplate(BlockTemplate::Ref templ) { _template = templ; }
	constexpr BlockTemplate::Ref getTemplate() const { return _template; }

	constexpr const Slot& getBlockSlot() const { return _slot; }

	inline void render(const Camera& cam) override { luaRender(cam); }

	constexpr glm::vec3 getMinimums() const { return getPosition() - glm::vec3(cubes::side::midsize); }
	constexpr glm::vec3 getMaximums() const { return getPosition() + glm::vec3(cubes::side::midsize); }

protected:
	Model::Ref internalGetModel() const override { return cubes::model::getModel(); }

public:
	constexpr Side& operator[] (Side::SideId sideId) { return getSide(sideId); }
	constexpr const Side& operator[] (Side::SideId sideId) const { return getSide(sideId); }

private:
	constexpr void setBlockId(Id blockId) { _blockId = blockId; }
	constexpr void setBlockSlot(const Slot& coords) { _slot = coords; }

public:
	static inline void setTransformOnSide(Transformable& transf, const Block& block, Side::SideId sideId, glm::vec3 offsets = {})
	{
		const auto& sideTransf = block.getSide(sideId).getTransform();
//		auto result = block * sideTransf;
		transf.setPosition(sideTransf.getPosition());
		transf.setRotation(sideTransf.getRotation());

		offsets = glm::quat(glm::radians(transf.getRotation())) * offsets;
		transf.move(offsets);
	}
};


inline BlockSide::Id BlockSide::getId() const { return (_parent->getBlockId() - 1) * cubes::side::count + cubes::side::idToInt(_sideId); }

inline BlockTemplate::Ref BlockSide::getTemplate() const { return _template ? _template : _parent->_template; }

inline void BlockTemplate::onRender(Block& block, const Camera& cam) { vcall(FunctionOnRender, std::addressof(block), std::addressof(cam)); }
inline void BlockTemplate::onRenderSide(BlockSide& side, const Camera& cam) { vcall(FunctionOnRenderSide, std::addressof(side), std::addressof(cam)); }
inline void BlockTemplate::onUpdate(Block& block, Time elapsedTime) { vcall(FunctionOnUpdate, std::addressof(block), elapsedTime.toSeconds()); }
inline void BlockTemplate::onUpdateSide(BlockSide& side, Time elapsedTime) { vcall(FunctionOnUpdateSide, std::addressof(side), elapsedTime.toSeconds()); }

inline void BlockTemplate::onBlockConstruct(Block& block) { vcall(FunctionOnBlockConstruct, std::addressof(block)); }
inline void BlockTemplate::onBlockSideConstruct(BlockSide& side) { vcall(FunctionOnBlockSideConstruct, std::addressof(side)); }







class BlocksNet
{
private:
	std::unordered_map<Block::Slot, std::shared_ptr<Block>> _net;

public:
	BlocksNet() = default;
	BlocksNet(const BlocksNet&) = default;
	BlocksNet(BlocksNet&&) noexcept = default;
	~BlocksNet() = default;

	BlocksNet& operator= (const BlocksNet&) = default;
	BlocksNet& operator= (BlocksNet&&) noexcept = default;

public:
	inline bool empty() const { return _net.empty(); }
	inline std::size_t size() const { return _net.size(); }

	inline std::shared_ptr<Block> getBlock(const Block::Slot& slot) const
	{
		const auto& it = _net.find(slot);
		if (it == _net.end())
			return nullptr;
		return it->second;
	}

	inline bool setBlock(const Block::Slot& slot, const std::shared_ptr<Block>& block, bool adjustBlockPosition = true)
	{
		if (_net.contains(slot))
		{
			logger::error("Cannot place block on [{}, {}, {}]. This slot is already ocuped.", slot.x, slot.y, slot.z);
			return false;
		}

		_net.insert({ slot, block });

		if (adjustBlockPosition)
		{
			block->setBlockSlot(slot);
			block->setPosition(slot.toPosition());
		}

		return true;
	}

	inline std::shared_ptr<Block> eraseBlock(const Block::Slot& slot)
	{
		const auto& it = _net.find(slot);
		if (it == _net.end())
			return nullptr;

		auto ptr = it->second;
		_net.erase(it);
		return ptr;
	}

	inline void clear()
	{
		_net.clear();
	}
};



class BlockContainer
{
public:
	friend BlockContainerIterator;
	friend ConstBlockContainerIterator;

public:
	using Net = BlocksNet;
	using Slot = Block::Slot;

	using iterator = BlockContainerIterator;
	using const_iterator = ConstBlockContainerIterator;

private:
	Net _net = {};
	std::vector<std::shared_ptr<Block>> _allocator = {};
	std::priority_queue<Block::Id> _unusedIds = {};
	std::shared_ptr<Block> _first = nullptr;
	std::shared_ptr<Block> _last = nullptr;
	std::shared_ptr<TransparentRenderList> _transparentRenderList = nullptr;

public:
	BlockContainer() = default;
	BlockContainer(const BlockContainer&) = delete;
	BlockContainer(BlockContainer&&) noexcept = default;

	BlockContainer& operator= (const BlockContainer&) = delete;
	BlockContainer& operator= (BlockContainer&&) noexcept = default;

	inline ~BlockContainer() { clear(); }

public:
	void clear();

	std::shared_ptr<Block> createBlock(const Slot& slot, const std::string& templateName);

	bool removeBlock(const Slot& slot);

	std::shared_ptr<Block> getBlockById(Block::Id blockId) const;
	Reference<Block::Side> getBlockSideBySideId(Block::Side::Id sideId) const;

	void render(const Camera& cam);
	void update(Time elapsedTime);

public:
	inline bool empty() const { return _first == nullptr; }
	inline std::size_t size() const { return _net.size(); }

	inline bool containsBlock(const Slot& slot) const { return _net.getBlock(slot) != nullptr; }
	inline std::shared_ptr<Block> getBlock(const Slot& slot) const { return _net.getBlock(slot); }

	inline std::shared_ptr<Block> operator[] (const Slot& slot) const { return getBlock(slot); }

	inline std::shared_ptr<Block> getBlockBySideId(Block::Side::Id sideId) const
	{
		return getBlockById(Block::Id(sideId / cubes::side::count) + 1);
	}

private:
	std::shared_ptr<Block> createNewBlockFromTemplate(const std::string& templateName);

private:
	static constexpr std::size_t bidToIdx(Block::Id id) { return static_cast<std::size_t>(id - 1); }
	static constexpr Block::Id idxToBid(std::size_t idx) { return static_cast<Block::Id>(idx + 1); }

public:
	iterator begin();
	const_iterator begin() const;
	const_iterator cbegin() const;
	iterator end();
	const_iterator end() const;
	const_iterator cend() const;
};


class BlockContainerIterator
{
public:
	friend ConstBlockContainerIterator;

public:
	using iterator_category = std::forward_iterator_tag;
	using difference_type = std::ptrdiff_t;
	using value_type = std::shared_ptr<Block>;
	using pointer = std::shared_ptr<Block>*;
	using reference = std::shared_ptr<Block>&;
	using const_pointer = const std::shared_ptr<Block>*;
	using const_reference = const std::shared_ptr<Block>&;

private:
	value_type _block = nullptr;

public:
	constexpr BlockContainerIterator() = default;
	BlockContainerIterator(const BlockContainerIterator&) = default;
	BlockContainerIterator(BlockContainerIterator&&) noexcept = default;
	~BlockContainerIterator() = default;

	BlockContainerIterator& operator= (const BlockContainerIterator&) = default;
	BlockContainerIterator& operator= (BlockContainerIterator&&) noexcept = default;

	bool operator== (const BlockContainerIterator&) const = default;

public:
	inline BlockContainerIterator(const_pointer ptr) : _block(*ptr) {}
	inline BlockContainerIterator(const_reference ref) : _block(ref) {}

	inline BlockContainerIterator& operator++ () { return _block = _block->_nextBlock, *this; }
	inline BlockContainerIterator operator++ (int) { auto old = *this; return ++(*this), old; }

	inline const value_type& operator* () const { return _block; }
	inline Block* operator-> () const { return _block.get(); }
};

class ConstBlockContainerIterator
{
public:
	using iterator_category = std::forward_iterator_tag;
	using difference_type = std::ptrdiff_t;
	using value_type = std::shared_ptr<const Block>;
	using pointer = std::shared_ptr<const Block>*;
	using reference = std::shared_ptr<const Block>&;
	using const_pointer = const std::shared_ptr<const Block>*;
	using const_reference = const std::shared_ptr<const Block>&;

private:
	value_type _block = nullptr;

public:
	constexpr ConstBlockContainerIterator() = default;
	ConstBlockContainerIterator(const ConstBlockContainerIterator&) = default;
	ConstBlockContainerIterator(ConstBlockContainerIterator&&) noexcept = default;
	~ConstBlockContainerIterator() = default;

	ConstBlockContainerIterator& operator= (const ConstBlockContainerIterator&) = default;
	ConstBlockContainerIterator& operator= (ConstBlockContainerIterator&&) noexcept = default;

	bool operator== (const ConstBlockContainerIterator&) const = default;

public:
	inline ConstBlockContainerIterator(const const_pointer ptr) : _block(*ptr) {}
	inline ConstBlockContainerIterator(const const_reference ref) : _block(ref) {}
	inline ConstBlockContainerIterator(const BlockContainerIterator::const_pointer ptr) : _block(*ptr) {}
	inline ConstBlockContainerIterator(const BlockContainerIterator::const_reference ref) : _block(ref) {}
	inline ConstBlockContainerIterator(BlockContainerIterator it) : _block(it._block) {}

	inline ConstBlockContainerIterator& operator++ () { return _block = _block->_nextBlock, *this; }
	inline ConstBlockContainerIterator operator++ (int) { auto old = *this; return ++(*this), old; }

	inline const value_type& operator* () const { return _block; }
	inline const Block* operator-> () const { return _block.get(); }
};

inline BlockContainer::iterator BlockContainer::begin() { return iterator(_first); }
inline BlockContainer::const_iterator BlockContainer::begin() const { return const_iterator(_first); }
inline BlockContainer::const_iterator BlockContainer::cbegin() const { return const_iterator(_first); }
inline BlockContainer::iterator BlockContainer::end() { return iterator(); }
inline BlockContainer::const_iterator BlockContainer::end() const { return const_iterator(); }
inline BlockContainer::const_iterator BlockContainer::cend() const { return const_iterator(); }
