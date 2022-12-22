#include "luadefs.h"

#include "math/glm.h"
#include "utils/logger.h"
#include "utils/luadebuglib.h"

#include "theme.h"


void lua::initGameLibs()
{
	static constinit bool initiatedFlag = false;

	if (!initiatedFlag)
	{
		lua::lib::registerGlmToLua();
		lua::lib::registerGlLibToLua();
		lua::lib::registerDebugLibToLua();
		lua::lib::registerCameraLibToLua();
		lua::lib::registerShaderLibToLua();
		lua::lib::registerEntitiesLibToLua();
		lua::lib::registerThemesLibToLua();
		lua::lib::registerTilesLibToLua();
		lua::lib::registerBlocksLibToLua();
		lua::lib::registerModelsLibToLua();
		lua::lib::registerBallsLibToLua();
		lua::lib::registerSkyboxessLibToLua();

		initiatedFlag = true;
	}
}


Reference<LuaRef> LuaTemplate::findLuaObject(std::string_view name) const
{
	if (!isLoaded())
		return nullptr;

	auto it = _luaCache.find(name.data());
	if (it != _luaCache.end())
		return it->second.get();

	auto obj = std::make_unique<LuaRef>(_module->getValue<LuaRef>(name));
	if (obj->isNil())
		return nullptr;

	Reference<LuaRef> ref = obj.get();
	_luaCache.insert({ name.data(), std::move(obj) });

	return ref;
}

LuaTemplate::~LuaTemplate()
{
	clear();
}

bool LuaTemplate::load()
{
	if (isLoaded())
	{
		logger::warn("Attempt to load already loaded LuaModel {}/{}.lua", getTemplateTypeName(getType()), _name);
		return false;
	}

	auto filePathOpt = findModelFile();
	if (!filePathOpt.has_value())
	{
		logger::error("LuaModel {} not found.", _name);
		return false;
	}

	_module = LuaModuleManager::instance().load(filePathOpt.value());
	if (_module == nullptr || !_module->isLoaded())
	{
		logger::error("Cannot load LuaModel Script {} because not found or has errors on load.", filePathOpt.value().string());
		return false;
	}

	static constinit Id _idgen = 1;

	_id = _idgen++;
	_luaCache.clear();

	init();

	return true;
}

void LuaTemplate::reload()
{
	if (isLoaded())
	{
		clear();
		_luaCache.clear();
		_module->reload();
		init();
	}
}

void LuaTemplate::clear()
{
	vcall(FunctionOnDestroy);
}

void LuaTemplate::init()
{
	vcall(FunctionOnInit);
}

std::optional<Path> LuaTemplate::findModelFile() const
{
	if (_name.empty())
		return {};

	Type type = getType();
	if (type == Type::Unknown)
		return {};

	std::string typeName = getTemplateTypeName(type).data();

	return resources::findFirstValidPath(resources::templates.path(), (Path(typeName) / _name).string(), ".lua");
}
