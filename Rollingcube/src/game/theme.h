#pragma once

#include <string>
#include <unordered_set>
#include <unordered_map>
#include <functional>

#include "utils/optref.h"

#include "luadefs.h"

#include "block.h"
#include "tile.h"


namespace lua::lib { void registerThemesLibToLua(); }


class Theme;

class ThemeModel : public LuaModel
{
public:
	using Ref = Reference<ThemeModel>;
	using ConstRef = ConstReference<ThemeModel>;

public:
	static constexpr std::string_view FunctionOnLoad = "OnLoad";

public:
	ThemeModel() = default;
	ThemeModel(const ThemeModel&) = delete;
	ThemeModel(ThemeModel&&) noexcept = default;
	~ThemeModel() = default;

	ThemeModel& operator= (const ThemeModel&) = delete;
	ThemeModel& operator= (ThemeModel&&) noexcept = default;

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
	std::unique_ptr<ThemeModel> _model = nullptr;

	mutable std::unordered_map<std::string, Reference<LuaModel>> _models[LuaModel::TypeCount] = {};

	mutable TextureManager _textureManager = TextureManager::root().createChildManager();

public:
	Theme(const Theme&) = delete;
	Theme(Theme&&) noexcept = delete;

	Theme& operator= (const Theme&) = delete;
	Theme& operator= (Theme&&) noexcept = delete;

private:
	Theme() = default;
	~Theme() = default;

public:
	constexpr bool isLoaded() const { return _model != nullptr; }

	constexpr const std::string& getName() const { return _name; }

	constexpr const std::string& getBaseDirectory() const { return _baseDir; }

public:
	inline Reference<TileModel> getTileModel(const std::string& name) const { return getModel<TileModel>(LuaModel::Type::Tile, name); }
	inline Reference<BlockModel> getBlockModel(const std::string& name) const { return getModel<BlockModel>(LuaModel::Type::Block, name); }

public:
	inline Tile getTile(const std::string& name) const { return getTileModel(name); }

public:
	Texture::Ref getTexture(const std::string& name) const;

private:
	bool load(const std::string& name);
	void loadModels();
	void unload();

	void scanDirectoryForScripts(const Path& directory, const std::function<void(const ScriptsDirectoryFileInfo&)>& action);
	void loadLuaModelData(LuaModel::Type type, const std::function<Reference<LuaModel>(const ScriptsDirectoryFileInfo&)>& loaderFunction);

private:
	inline std::string prepareElementName(const std::string& name) const { return (Path(getBaseDirectory()) / name).string(); }

public:
	static const std::unordered_set<std::string>& getAvailableThemeFiles();

public:
	static constexpr const Theme& getCurrentTheme() { return CurrentTheme; }

	static bool changeCurrentTheme(const std::string& theme);
	static void releaseCurrentTheme();

private:
	template <std::derived_from<LuaModel> _Ty>
	void loadLuaModelData(LuaModel::Type type, LuaModelManager<_Ty>& manager)
	{
		auto loaderFn = [&manager](const ScriptsDirectoryFileInfo& fileinfo) -> Reference<LuaModel> {
			auto ref = manager.load(fileinfo.relativePath);
			if (ref == nullptr)
				return nullptr;

			return static_cast<LuaModel*>(&ref);
		};

		loadLuaModelData(type, loaderFn);
	}

	inline std::unordered_map<std::string, Reference<LuaModel>>& getModelCache(LuaModel::Type type) const
	{
		return _models[static_cast<int>(type)];
	}

	template <std::derived_from<LuaModel> _ModelTy>
	inline Reference<_ModelTy> getModel(LuaModel::Type type, const std::string& name) const
	{
		auto& modelCache = getModelCache(type);
		auto it = modelCache.find(name);
		if (it == modelCache.end())
			return nullptr;

		return reinterpret_cast<_ModelTy*>(&it->second);
	}
};
