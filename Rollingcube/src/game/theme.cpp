#include "theme.h"

#include <unordered_set>

#include "utils/resources.h"

#include "engine/lua/module.h"
#include "utils/lualib_constants.h"


Theme Theme::CurrentTheme;

std::unordered_set<std::string> Theme::AvailableThemeFiles;

const std::unordered_set<std::string>& Theme::getAvailableThemeFiles()
{
	static constinit bool initiated = false;
	if (!initiated)
	{
		Path dir = resources::absolute(resources::templates / LuaTemplate::getTemplateTypeName(LuaTemplate::Type::Theme));
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

	auto model = std::make_unique<ThemeTemplate>();
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
	_template = std::move(model);

	_template->onLoad();

	ShaderProgramManager::instance().loadInternalShaders();
	loadTemplates();

	return true;
}

void Theme::loadTemplates()
{
	loadLuaTemplateData(LuaTemplate::Type::Tile, TileTemplateManager::instance());
	loadLuaTemplateData(LuaTemplate::Type::Block, BlockTemplateManager::instance());
	loadLuaTemplateData(LuaTemplate::Type::Model, ModelObjectTemplateManager::instance());
	loadLuaTemplateData(LuaTemplate::Type::Ball, BallTemplateManager::instance());
}

void Theme::unload()
{
	if (!isLoaded())
		return;

	_name = {};
	_baseDir = {};
	_template.release();
	
	for (std::size_t i = 0; i < LuaTemplate::TypeCount; ++i)
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

void Theme::loadLuaTemplateData(LuaTemplate::Type type, const std::function<Reference<LuaTemplate>(const ScriptsDirectoryFileInfo&)>& loaderFunction)
{
	auto& modelsCache = getTemplateCache(type);
	modelsCache.clear();

	scanDirectoryForScripts(resources::templates / LuaTemplate::getTemplateTypeName(type), [&modelsCache, &loaderFunction](const ScriptsDirectoryFileInfo& fileinfo) {
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

CubeMapTexture::Ref Theme::getCubeMapTexture(const std::string& name) const
{
	auto ref = _cubeMapTextureManager.get(name);
	if (ref != nullptr)
		return ref;

	std::string relativeFilePath = prepareElementName(name);

	static const std::initializer_list<std::string_view> textureExtensions = { ".json" };

	auto opath = resources::findFirstValidPath(resources::textures.path(), relativeFilePath, textureExtensions);
	if (!opath.has_value())
	{
		logger::error("Texture on path {} not found.", (resources::textures.path() / relativeFilePath).string());
		return nullptr;
	}

	return _cubeMapTextureManager.loadFromJson(name, std::string_view(opath.value().string()), resources::cubemapTextures.string());
}

Model::Ref Theme::getModel(const std::string& name) const
{
	auto ref = _modelManager.get(name);
	if (ref != nullptr)
		return ref;

	std::string relativeFilePath = prepareElementName(name);

	static const std::initializer_list<std::string_view> textureExtensions = { ".obj" };

	auto opath = resources::findFirstValidPath(resources::models.path(), relativeFilePath, textureExtensions);
	if (!opath.has_value())
	{
		logger::error("Model on path {} not found.", (resources::models.path() / relativeFilePath).string());
		return nullptr;
	}

	return _modelManager.loadFromFile(name, opath.value().string(), true);
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
	static CubeMapTexture* getCubeMapTexture(const std::string& name) { return &Theme::getCurrentTheme().getCubeMapTexture(name); }

	static Model* getModel(const std::string& name) { return &Theme::getCurrentTheme().getModel(name); }
	static Model* getBallModel() { return &Theme::getCurrentTheme().getBallModel(); }
	static Model* getSkyboxModel() { return &Theme::getCurrentTheme().getSkyboxModel(); }

	static Tile getTile(const std::string& name) { return Theme::getCurrentTheme().getTile(name); }
	static ModelObject getModelObject(const std::string& name) { return Theme::getCurrentTheme().getModelObject(name); }


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
			.addStaticFunction("getCubeMapTexture", &getCubeMapTexture)
			.addStaticFunction("getModel", &getModel)
			.addStaticFunction("getBallModel", &getBallModel)
			.addStaticFunction("getSkyboxModel", &getSkyboxModel)
			.addStaticFunction("getTile", &getTile)
			.addStaticFunction("getModelObject", &getModelObject)
			;

		lua::lib::addLocalValuesContainerToClass(clss);

		root = clss.endClass();
		return true;
	}
}
