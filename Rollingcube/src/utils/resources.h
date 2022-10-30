#pragma once

#include <functional>
#include <filesystem>
#include <optional>
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

		inline const Path& path() const { return _dirpath; }

		inline Path resolve(std::string_view subpath) const { return _dirpath / subpath; }
		inline Path resolve(const Path& subpath) const { return _dirpath / subpath; }
		inline Path operator/ (std::string_view subpath) const { return _dirpath / subpath; }
		inline Path operator/ (const Path& subpath) const { return _dirpath / subpath; }

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

	inline Path absolute(const Path& path) { return std::filesystem::absolute(path); }

	std::optional<Path> findFirstValidPath(const Path& root, std::string_view name, std::initializer_list<std::string_view> extensions);

	inline std::optional<Path> findFirstValidPath(const Path& root, std::string_view name, std::string_view extension)
	{
		return findFirstValidPath(root, name, { extension });
	}

	void scanDirectory(const Path& directory, const std::function<void(const Path&)>& action);

	void scanDirectoryFiles(const Path& directory, std::initializer_list<std::string_view> extensions, const std::function<void(const Path&)>& action);

	inline void scanDirectoryFiles(const Path& directory, std::string_view extension, const std::function<void(const Path&)>& action)
	{
		scanDirectoryFiles(directory, { extension }, action);
	}
}

namespace resources
{
	inline const Directory data = { "data" };
	inline const Directory shaders = { data, "shaders" };
	inline const Directory fonts = { data, "fonts" };
	inline const Directory defs = { data, "defs" };
	inline const Directory textures = { data, "textures" };
}
