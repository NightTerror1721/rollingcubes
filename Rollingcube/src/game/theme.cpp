#include "theme.h"

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
		Path dir = resources::absolute(resources::models / LuaModel::getModelTypeName(LuaModel::Type::Theme));
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

	return true;
}

void Theme::unload()
{
	if (!isLoaded())
		return;

	_name = {};
	_baseDir = {};
	_model.release();

	_texturesCache.clear();
	_tilesCache.clear();
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
	auto optionalRef = _texturesCache.opt(name);
	if (optionalRef)
		return *optionalRef;

	std::string relativeFilePath = prepareElementName(name);
	std::string key = "Theme::" + _name + relativeFilePath;

	Texture::Ref ref = TextureManager::root().get(key);
	if (ref == nullptr)
	{
		static const std::initializer_list<std::string_view> textureExtensions = { ".jpg", ".png", ".bmp" };

		auto opath = resources::findFirstValidPath(resources::textures.path(), relativeFilePath, textureExtensions);
		if (!opath.has_value())
		{
			logger::error("Texture on path {} not found.", (resources::textures.path() / relativeFilePath).string());
			return nullptr;
		}

		ref = TextureManager::root().loadFromImage(key, opath.value().string());
		if (ref == nullptr)
			return nullptr;
	}

	_texturesCache.insert(name, ref);
	return ref;
}

Tile Theme::getTile(const std::string& name) const
{
	auto optionalRef = _tilesCache.opt(name);
	if (optionalRef)
		return *optionalRef;

	TileModel::Ref ref = TileModelManager::instance().load(prepareElementName(name));
	if (ref == nullptr)
		return Tile();

	_tilesCache.insert(name, ref);
	return ref;
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
