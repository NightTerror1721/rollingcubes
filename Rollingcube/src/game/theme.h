#pragma once

#include <string>
#include <unordered_set>
#include <unordered_map>
#include <functional>

#include "utils/optref.h"

#include "luadefs.h"

#include "block.h"
#include "tile.h"
#include "modelobj.h"
#include "ball.h"


namespace lua::lib { void registerThemesLibToLua(); }


class Theme;

class ThemeTemplate : public LuaTemplate
{
public:
	using Ref = Reference<ThemeTemplate>;
	using ConstRef = ConstReference<ThemeTemplate>;

public:
	static constexpr std::string_view FunctionOnLoad = "OnLoad";

public:
	ThemeTemplate() = default;
	ThemeTemplate(const ThemeTemplate&) = delete;
	ThemeTemplate(ThemeTemplate&&) noexcept = default;
	~ThemeTemplate() = default;

	ThemeTemplate& operator= (const ThemeTemplate&) = delete;
	ThemeTemplate& operator= (ThemeTemplate&&) noexcept = default;

	inline Type getType() const override { return Type::Theme; }

public:
	inline void onLoad() { vcall(FunctionOnLoad); }

public:
	friend Theme;
};




class Theme : public LuaLocalVariablesContainer
{
private:
	static Theme CurrentTheme;

	static std::unordered_set<std::string> AvailableThemeFiles;

private:
	template <typename _Ty>
	class Cache
	{
	private:
		std::unordered_map<std::string, _Ty> _cache;

	public:
		Cache() = default;
		Cache(const Cache&) = delete;
		Cache(Cache&&) noexcept = delete;
		~Cache() = default;

		Cache& operator= (const Cache&) = delete;
		Cache& operator= (Cache&&) noexcept = delete;

	public:
		inline bool contains(const std::string& name) const { return _cache.contains(name); }
		inline optref<_Ty> opt(const std::string& name)
		{
			auto it = _cache.find(name);
			if (it == _cache.end())
				return optref<_Ty>::empty();
			return optref<_Ty>::of(it->second);
		}
		inline const_optref<_Ty> opt(const std::string& name) const
		{
			auto it = _cache.find(name);
			if (it == _cache.end())
				return const_optref<_Ty>::empty();
			return const_optref<_Ty>::of(it->second);
		}
		inline _Ty& get(const std::string& name) { return _cache.at(name); }
		inline const _Ty& get(const std::string& name) const { return _cache.at(name); }
		inline void insert(const std::string& name, const _Ty& value) { _cache.insert({ name, value }); }
		inline void insert(const std::string& name, _Ty&& value) { _cache.insert({ name, std::move(value) }); }
		inline void clear() { _cache.clear(); }
	};

	template <typename _Ty>
	using CacheRef = Cache<Reference<_Ty>>;

private:
	struct ScriptsDirectoryFileInfo
	{
		const std::string& keyname;
		const std::string& relativePath;
		const Path& path;
	};

private:
	std::string _name = {};
	std::string _baseDir = {};
	std::unique_ptr<ThemeTemplate> _template = nullptr;

	mutable std::unordered_map<std::string, Reference<LuaTemplate>> _models[LuaTemplate::TypeCount] = {};

	mutable TextureManager _textureManager = TextureManager::root().createChildManager();
	mutable ModelManager _modelManager = ModelManager::root().createChildManager();

public:
	Theme(const Theme&) = delete;
	Theme(Theme&&) noexcept = delete;

	Theme& operator= (const Theme&) = delete;
	Theme& operator= (Theme&&) noexcept = delete;

private:
	Theme() = default;
	~Theme() = default;

public:
	constexpr bool isLoaded() const { return _template != nullptr; }

	constexpr const std::string& getName() const { return _name; }

	constexpr const std::string& getBaseDirectory() const { return _baseDir; }

public:
	inline Reference<TileTemplate> getTileTemplate(const std::string& name) const { return getTemplate<TileTemplate>(LuaTemplate::Type::Tile, name); }
	inline Reference<BlockTemplate> getBlockTemplate(const std::string& name) const { return getTemplate<BlockTemplate>(LuaTemplate::Type::Block, name); }
	inline Reference<ModelObjectTemplate> getModelObjectTemplate(const std::string& name) const { return getTemplate<ModelObjectTemplate>(LuaTemplate::Type::Model, name); }
	inline Reference<BallTemplate> getBallTemplate(const std::string& name) const { return getTemplate<BallTemplate>(LuaTemplate::Type::Ball, name); }

public:
	inline Tile getTile(const std::string& name) const { return getTileTemplate(name); }
	inline ModelObject getModelObject(const std::string& name) const { return getModelObjectTemplate(name); }

public:
	Texture::Ref getTexture(const std::string& name) const;
	Model::Ref getModel(const std::string& name) const;

public:
	inline Model::Ref getBallModel() const { return balls::model::getModel(); }

private:
	bool load(const std::string& name);
	void loadTemplates();
	void unload();

	void scanDirectoryForScripts(const Path& directory, const std::function<void(const ScriptsDirectoryFileInfo&)>& action);
	void loadLuaTemplateData(LuaTemplate::Type type, const std::function<Reference<LuaTemplate>(const ScriptsDirectoryFileInfo&)>& loaderFunction);

private:
	inline std::string prepareElementName(const std::string& name) const { return (Path(getBaseDirectory()) / name).string(); }

public:
	static const std::unordered_set<std::string>& getAvailableThemeFiles();

public:
	static constexpr const Theme& getCurrentTheme() { return CurrentTheme; }

	static bool changeCurrentTheme(const std::string& theme);
	static void releaseCurrentTheme();

private:
	template <std::derived_from<LuaTemplate> _Ty>
	void loadLuaTemplateData(LuaTemplate::Type type, LuaTemplateManager<_Ty>& manager)
	{
		auto loaderFn = [&manager](const ScriptsDirectoryFileInfo& fileinfo) -> Reference<LuaTemplate> {
			auto ref = manager.load(fileinfo.relativePath);
			if (ref == nullptr)
				return nullptr;

			return static_cast<LuaTemplate*>(&ref);
		};

		loadLuaTemplateData(type, loaderFn);
	}

	inline std::unordered_map<std::string, Reference<LuaTemplate>>& getTemplateCache(LuaTemplate::Type type) const
	{
		return _models[static_cast<int>(type)];
	}

	template <std::derived_from<LuaTemplate> _ModelTy>
	inline Reference<_ModelTy> getTemplate(LuaTemplate::Type type, const std::string& name) const
	{
		auto& modelCache = getTemplateCache(type);
		auto it = modelCache.find(name);
		if (it == modelCache.end())
			return nullptr;

		return reinterpret_cast<_ModelTy*>(&it->second);
	}
};
