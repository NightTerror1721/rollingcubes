#pragma once

#include <optional>

#include "core/window.h"

#include "utils/resources.h"
#include "utils/optref.h"
#include "utils/logger.h"
#include "utils/reference.h"


template <typename _Ty>
concept ValidPropertyValue =
	std::integral<_Ty> ||
	std::floating_point<_Ty> ||
	std::same_as<bool, _Ty> ||
	std::same_as<std::string, _Ty> ||
	std::same_as<JsonArray, _Ty> ||
	std::same_as<JsonObject, _Ty>;

namespace utils
{
	class PropsJson
	{
	private:
		Reference<JsonObject> _json;

	public:
		inline PropsJson(JsonObject& json) : _json(std::addressof(json)) {}

	public:
		template <typename _Ty> requires std::integral<_Ty> || std::floating_point<_Ty> || std::same_as<bool, _Ty>
		inline _Ty get(std::string_view name, _Ty defval = _Ty()) const
		{
			const auto& it = _json->find(name);
			if (it != _json->end())
				return it->second.get<_Ty>();
			return defval;
		}

		template <typename _Ty> requires std::same_as<JsonArray, _Ty> || std::same_as<JsonObject, _Ty>
		inline _Ty& get(std::string_view name, const _Ty& defval = _Ty()) const
		{
			const auto& it = _json->find(name);
			if (it != _json->end())
				return it->second.get_ref<_Ty&>();
			return defval;
		}

		template <typename _Ty> requires std::integral<_Ty> || std::floating_point<_Ty> || std::same_as<bool, _Ty>
		inline std::optional<_Ty> opt(std::string_view name) const
		{
			const auto& it = _json->find(name);
			if (it != _json->end())
				return it->second.get<_Ty>();
			return {};
		}

		template <typename _Ty> requires std::same_as<JsonArray, _Ty> || std::same_as<JsonObject, _Ty>
		inline optref<_Ty> opt(std::string_view name) const
		{
			const auto& it = _json->find(name);
			if (it != _json->end())
				return optref<_Ty>::of(it->second.get_ref<_Ty&>());
			return optref<_Ty>::empty();
		}

		inline bool has(std::string_view name) const { return _json->contains(name.data()); }

		template <typename _Ty> requires std::integral<_Ty> || std::floating_point<_Ty> || std::same_as<bool, _Ty>
		inline void set(std::string_view name, _Ty value)
		{
			_json->insert({ std::string(name), value });
		}

		template <typename _Ty> requires std::same_as<JsonArray, _Ty> || std::same_as<JsonObject, _Ty>
		inline void set(std::string_view name, const _Ty& value)
		{
			_json->insert({ std::string(name), value });
		}

		template <typename _Ty> requires std::same_as<JsonArray, _Ty> || std::same_as<JsonObject, _Ty>
		inline void set(std::string_view name, _Ty&& value)
		{
			_json->insert({ std::string(name), std::move(value) });
		}
	};
}

class Properties
{
private:
	static Properties Instance;

	static constexpr std::string_view FileName = "props.json";

public:
	Properties(const Properties&) = delete;
	Properties(Properties&&) noexcept = delete;

	Properties& operator= (const Properties&) = delete;
	Properties& operator= (Properties&&) noexcept = delete;

private:
	Properties() = default;
	~Properties() = default;

public:
	static constexpr Properties& instance() noexcept { return Instance; }
	static constexpr Reference<Properties> referenceInstance() noexcept { return std::addressof(Instance); }

	static void load()
	{
		Path path = resources::user / FileName;
		if (!std::filesystem::is_regular_file(path))
			return;

		try
		{
			JsonValue value = json::read(path.string());
			if (!value.is_object())
				return;

			Instance.load(value.get_ref<JsonObject&>());
		}
		catch (const std::exception& ex)
		{
			logger::error("Exception during Properties load: {}.", ex.what());
		}
	}

	static void save()
	{
		JsonObject obj;
		Instance.save(obj);
		resources::user.writeJson(FileName, obj);
	}

public:
	unsigned int windowWidth = window::default_width;
	unsigned int windowHeight = window::default_height;

private:
	void load(const utils::PropsJson json)
	{
		windowWidth = json.get<unsigned int>("window.width", window::default_width);
		windowHeight = json.get<unsigned int>("window.height", window::default_height);
	}

	void save(utils::PropsJson json)
	{
		json.set("window.width", windowWidth);
		json.set("window.height", windowHeight);
	}
};

inline Properties Properties::Instance = {};
