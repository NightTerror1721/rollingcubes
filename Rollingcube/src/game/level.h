#pragma once

#include "theme.h"


class Level : public LocalValuesContainer
{
private:
	BlockContainer _blocks = {};
	AABB _limits = {};
	Path _filePath = {};
	std::shared_ptr<TransparentRenderList> _transparentRenderList = nullptr;

public:
	Level() = default;
	Level(const Level&) = delete;
	Level(Level&&) noexcept = default;

	Level& operator= (const Level&) = delete;
	Level& operator= (Level&&) noexcept = default;

public:
	void render(const Camera& cam);
	void update(Time elapsedTime);
	void dispatchEvent(const InputEvent& event);

	void clear();

	void computeLimits();

public:
	constexpr BlockContainer& getBlockContainer() { return _blocks; }
	constexpr const BlockContainer& getBlockContainer() const { return _blocks; }

	constexpr const AABB& getLimits() const { return _limits; }

	constexpr const Path& getFilePath() const { return _filePath; }

	inline void setTransparentRenderList(const std::shared_ptr<TransparentRenderList>& list) { _transparentRenderList = list; }


	inline std::shared_ptr<Block> insertBlock(const Block::Slot& slot, const std::string& templateName) { return _blocks.createBlock(slot, templateName); }
	inline bool removeBlock(const Block::Slot& slot) { return _blocks.removeBlock(slot); }
	inline std::shared_ptr<Block> getBlock(const Block::Slot& slot) { return _blocks.getBlock(slot); }
};
