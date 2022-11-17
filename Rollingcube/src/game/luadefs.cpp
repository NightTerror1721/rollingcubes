#include "luadefs.h"

#include "utils/logger.h"


Reference<LuaRef> LuaTemplate::findLuaObject(std::string_view name) const
{
	if (!_loaded)
		return nullptr;

	auto it = _luaCache.find(name.data());
	if (it != _luaCache.end())
		return it->second.get();

	auto obj = std::make_unique<LuaRef>(_script.getEnv()[name]);
	if (obj->isNil())
	{
		obj.release();
		return nullptr;
	}

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

	LuaScript script = LuaScriptManager::instance().getScript(filePathOpt.value().string());
	if (!script)
	{
		logger::error("Cannot load LuaModel Script {} not found.", filePathOpt.value().string());
		return false;
	}

	static constinit Id _idgen = 1;

	_id = _idgen++;
	_script = script;
	_luaCache.clear();
	_loaded = true;

	init();

	return true;
}

void LuaTemplate::reload()
{
	if (isLoaded())
	{
		clear();
		_luaCache.clear();
		_script.reload();
		init();
	}
}

void LuaTemplate::clear()
{
	vcall(FunctionOnDestroy);
}

void LuaTemplate::init()
{
	_script();
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
