#pragma once

#include <string_view>
#include <string>
#include <format>
#include <vector>


namespace utils::str
{
	template <typename... _ArgsTys>
	inline std::string format(std::string_view fmt, _ArgsTys&&... args)
	{
		return std::vformat(fmt, std::make_format_args(std::forward<_ArgsTys>(args)...));
	}

	inline std::vector<std::string> split(std::string str, char separator)
	{
        std::vector<std::string> res;
        while (true)
        {
            auto pos = str.find(separator);
            if (pos == -1)
            {
                if (str.size() > 0)
                    res.push_back(str);
                break;
            }
            std::string tp = str.substr(0, pos);
            if (tp.size() != 0)
                res.push_back(tp);
            str = str.substr(pos + 1, str.size() - pos - 1);
        }
        return res;
	}
    inline std::vector<std::string> split(std::string_view str, char separator) { return split(std::string(str), separator); }
}
