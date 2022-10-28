#include "resources.h"


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
}
