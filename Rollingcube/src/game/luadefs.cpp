#include "luadefs.h"

#include "utils/logger.h"


Reference<LuaRef> LuaModel::findLuaObject(std::string_view name) const
{
	if (!_loaded)
		return nullptr;

	auto it = _luaCache.find(name.data());
	if (it != _luaCache.end())
		return it->second.get();

	auto obj = std::make_unique<LuaRef>(_script.getEnvValue(name));
	if (obj->isNil())
	{
		obj.release();
		return nullptr;
	}

	Reference<LuaRef> ref = obj.get();
	_luaCache.insert({ name.data(), std::move(obj) });

	return ref;
}

LuaModel::~LuaModel()
{
	clear();
}

bool LuaModel::load()
{
	if (isLoaded())
	{
		logger::warn("Attempt to load already loaded LuaModel {}/{}.lua", getModelTypeName(getType()), _name);
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

void LuaModel::reload()
{
	if (isLoaded())
	{
		clear();
		_luaCache.clear();
		_script.reload();
		init();
	}
}

void LuaModel::clear()
{
	vcall(FunctionOnDestroy);
}

void LuaModel::init()
{
	_script();
	vcall(FunctionOnInit);
}

std::optional<Path> LuaModel::findModelFile() const
{
	if (_name.empty())
		return {};

	Type type = getType();
	if (type == Type::Unknown)
		return {};

	std::string typeName = getModelTypeName(type).data();

	return resources::findFirstValidPath(resources::defs.path(), (Path(typeName) / _name).string(), ".lua");

	/*if (_name.empty())
		return {};

	Type type = getType();
	if (type == Type::Unknown)
		return {};

	std::string typeName = getModelTypeName(type).data();

	const Path relative = Path(typeName) / _name;
	int count = int(std::distance(relative.begin(), relative.end()));

	Path path = resources::models / relative;
	for(; count > 0; --count)
	{
		Path file = resources::absolute(path);
		file += ".lua";

		if (std::filesystem::exists(file) && std::filesystem::is_regular_file(file))
			return resources::absolute(file);

		if (path.has_parent_path())
			path = path.parent_path();
		else
			count = 0;
	}

	return {};*/
}
