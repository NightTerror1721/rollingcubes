#include "theme.h"

#include <unordered_set>

#include "utils/resources.h"

#include "engine/lua/lua.h"
#include "engine/lua/constants.h"
#include "utils/lualib_constants.h"


Theme Theme::CurrentTheme;

std::unordered_set<std::string> Theme::AvailableThemeFiles;

const std::unordered_set<std::string>& Theme::getAvailableThemeFiles()
{
	static constinit bool initiated = false;
	if (!initiated)
	{
		Path dir = resources::absolute(resources::defs / LuaModel::getModelTypeName(LuaModel::Type::Theme));
		for (const auto& entry : std::filesystem::directory_iterator(dir))
		{
			if (entry.is_regular_file())
			{
				Path filePath = entry.path();
				if (filePath.has_extension() && filePath.extension() == ".lua")
				{
					Path temp = filePath;
					AvailableThemeFiles.insert(temp.replace_extension().filename().string());
				}

			}
		}

		initiated = true;
	}

	return AvailableThemeFiles;
}

bool Theme::load(const std::string& name)
{
	if (!getAvailableThemeFiles().contains(name))
	{
		logger::error("Theme {} not found.", name);
		return false;
	}

	auto model = std::make_unique<ThemeModel>();
	model->setName(name);
	if (!model->load())
	{
		logger::error("Cannot load Theme {}.", name);
		return false;
	}

	if (isLoaded())
		unload();

	_name = name;
	_baseDir = name;
	_model = std::move(model);

	_model->onLoad();

	loadModels();

	return true;
}

void Theme::loadModels()
{
	loadLuaModelData(LuaModel::Type::Tile, TileModelManager::instance());
	loadLuaModelData(LuaModel::Type::Block, BlockModelManager::instance());
}

void Theme::unload()
{
	if (!isLoaded())
		return;

	_name = {};
	_baseDir = {};
	_model.release();
	
	for (std::size_t i = 0; i < LuaModel::TypeCount; ++i)
		_models[i].clear();

	_textureManager.clear();
}

void Theme::scanDirectoryForScripts(const Path& directory, const std::function<void(const ScriptsDirectoryFileInfo&)>& action)
{
	namespace fs = std::filesystem;
	if (!fs::is_directory(directory))
		return;

	std::unordered_set<std::string> loaded;

	Path relative = Path(getBaseDirectory());
	int count = int(std::distance(relative.begin(), relative.end()));

	Path path = directory / relative;
	for (; count >= 0; --count)
	{
		resources::scanDirectoryFiles(path, ".lua", [&loaded, &relative, &action](const Path& file) {
			const std::string keyname = file.filename().replace_extension("").string();
			if (!loaded.contains(keyname))
			{
				action({ keyname, (relative / keyname).string(), fs::absolute(file)});
				loaded.insert(keyname);
			}
		});

		if (path.has_parent_path())
		{
			path = path.parent_path();
			relative = relative.parent_path();
		}
		else
			count = -1;
	}
}

void Theme::loadLuaModelData(LuaModel::Type type, const std::function<Reference<LuaModel>(const ScriptsDirectoryFileInfo&)>& loaderFunction)
{
	auto& modelsCache = getModelCache(type);
	modelsCache.clear();

	scanDirectoryForScripts(resources::defs / LuaModel::getModelTypeName(type), [&modelsCache, &loaderFunction](const ScriptsDirectoryFileInfo& fileinfo) {
		auto ref = loaderFunction(fileinfo);
		if (ref != nullptr)
			modelsCache.insert({ fileinfo.keyname, ref });
	});
}

bool Theme::changeCurrentTheme(const std::string& theme)
{
	return CurrentTheme.load(theme);
}

void Theme::releaseCurrentTheme()
{
	CurrentTheme.unload();
}




Texture::Ref Theme::getTexture(const std::string& name) const
{
	auto ref = _textureManager.get(name);
	if (ref != nullptr)
		return ref;
	
	std::string relativeFilePath = prepareElementName(name);

	static const std::initializer_list<std::string_view> textureExtensions = { ".jpg", ".png", ".bmp" };

	auto opath = resources::findFirstValidPath(resources::textures.path(), relativeFilePath, textureExtensions);
	if (!opath.has_value())
	{
		logger::error("Texture on path {} not found.", (resources::textures.path() / relativeFilePath).string());
		return nullptr;
	}

	return _textureManager.loadFromImage(name, opath.value().string());
}















namespace lua::lib
{
	namespace LUA_theme { static defineLuaLibraryConstructor(registerToLua, root, state); }

	void registerThemesLibToLua()
	{
		LuaLibraryManager::instance().registerLibrary(
			::lua::lib::names::themes,
			&LUA_theme::registerToLua,
			{ ::lua::lib::names::entities }
		);
	}
}

namespace lua::lib::LUA_theme
{
	static const std::string& getName() { return Theme::getCurrentTheme().getName(); }


	static bool change(const std::string& name) { return Theme::getCurrentTheme().changeCurrentTheme(name); }

	static Texture* getTexture(const std::string& name) { return &Theme::getCurrentTheme().getTexture(name); }

	static Tile getTile(const std::string& name) { return Theme::getCurrentTheme().getTile(name); }


	static defineLuaLibraryConstructor(registerToLua, root, state)
	{
		namespace meta = lua::metamethod;

		auto clss = root.beginClass<Theme>("Theme");
		clss
			// Fields //
			.addStaticProperty("name", &getName)
			// Methods //
			.addStaticFunction("change", &change)
			.addStaticFunction("getTexture", &getTexture)
			.addStaticFunction("getTile", &getTile)
			;

		lua::lib::utils::addLuaLocalVariablesToClass(clss);

		root = clss.endClass();
		return true;
	}
}
