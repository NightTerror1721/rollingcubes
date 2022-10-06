#pragma once

#include <functional>
#include <filesystem>
#include <compare>
#include <string>

#include "io_utils.h"
#include "json.h"


using Path = std::filesystem::path;

namespace resources
{
	class Directory
	{
	private:
		Path _dirpath;

	public:
		Directory(const Directory&) = default;
		Directory(Directory&&) noexcept = default;
		~Directory() = default;

		Directory& operator= (const Directory&) = default;
		Directory& operator= (Directory&&) noexcept = default;

		bool operator== (const Directory&) const = default;

	public:
		inline Directory(const std::string_view& root) : _dirpath(root) {}
		inline Directory(const Directory& root, const std::string_view& subpath) : _dirpath(root._dirpath / subpath) {}

		inline Path resolve(std::string_view subpath) const { return _dirpath / subpath; }
		inline Path operator/ (std::string_view subpath) const { return _dirpath / subpath; }

		inline std::ifstream openInputStream(std::string_view subpath) const { return std::ifstream(resolve(subpath)); }
		inline std::ofstream openOutputStream(std::string_view subpath) const { return std::ofstream(resolve(subpath)); }


		inline JsonValue readJson(std::string_view subpath) const
		{
			auto is = openInputStream(subpath);
			return json::read(is);
		}

		inline void writeJson(std::string_view subpath, const JsonValue& json) const
		{
			auto os = openOutputStream(subpath);
			json::write(os, json);
		}
	};
}

namespace resources
{
	inline const Directory data = { "data" };
	inline const Directory shaders = { data, "shaders" };
}
