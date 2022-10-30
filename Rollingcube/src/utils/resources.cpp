#include "resources.h"

#include <unordered_set>


namespace resources
{
	std::optional<Path> findFirstValidPath(const Path& root, std::string_view name, std::initializer_list<std::string_view> extensions)
	{
		if (name.empty())
			return {};

		const Path relative = Path(name);
		int count = int(std::distance(relative.begin(), relative.end()));

		Path path = root / relative;
		const Path filename = path.filename();
		for (; count > 0; --count)
		{
			Path file = resources::absolute(path);
			for (const auto& extension : extensions)
			{
				file.replace_extension(extension);

				if (std::filesystem::exists(file) && std::filesystem::is_regular_file(file))
					return resources::absolute(file);
			}

			if (path.has_parent_path())
				path = path.parent_path().replace_filename(filename);
			else
				count = 0;
		}

		return {};
	}

	void scanDirectory(const Path& directory, const std::function<void(const Path&)>& action)
	{
		namespace fs = std::filesystem;
		if (fs::is_directory(directory))
			for (const auto& entry : fs::directory_iterator(directory))
				action(entry.path());
	}

	void scanDirectoryFiles(const Path& directory, std::initializer_list<std::string_view> initExtensions, const std::function<void(const Path&)>& action)
	{
		namespace fs = std::filesystem;
		std::unordered_set<std::string_view> ext{ initExtensions.begin(), initExtensions.end() };
		scanDirectory(directory, [&ext, &action](const Path& entryPath) {
			if (fs::is_regular_file(entryPath))
			{
				const std::string extension = entryPath.extension().string();
				if (ext.contains(extension))
					action(entryPath);
			}
			});
	}
}
