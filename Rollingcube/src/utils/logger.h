#pragma once

#include <string>
#include <format>

#include "io_utils.h"

namespace logger
{
	enum class level
	{
		debug,
		info,
		warn,
		error,
		fatal
	};

	inline constexpr level system_level = level::warn;
}



namespace logger
{
	void log(level log_level, std::string_view msg);


	template <level _Level>
	struct loggerbuf
	{
		static constexpr level lv = _Level;

		constexpr loggerbuf() = default;
		constexpr loggerbuf(const loggerbuf&) = delete;
		constexpr loggerbuf& operator= (const loggerbuf&) = delete;
		constexpr ~loggerbuf() = default;

		inline void operator() (std::string_view msg) const { if constexpr (system_level <= lv) log(lv, msg); }

		template <typename... _ArgTys>
		inline void operator() (std::string_view fmt, _ArgTys&&... args) const
		{
			if constexpr (system_level <= lv)
			{
				log(lv, std::vformat(fmt, std::make_format_args(std::forward<_ArgTys>(args)...)));
			}
		}

		template <typename _Ty> requires requires(std::stringstream& ss, const _Ty& v)
		{
			{ ss << v };
		}
		inline const loggerbuf& operator<< (const _Ty& right) const
		{
			if constexpr (system_level <= lv)
			{
				std::stringstream ss;
				ss << right;
				return log(lv, ss.str()), *this;
			}
		}
	};

	inline constexpr const loggerbuf<level::debug> debug;
	inline constexpr const loggerbuf<level::info> info;
	inline constexpr const loggerbuf<level::warn> warn;
	inline constexpr const loggerbuf<level::error> error;
	inline constexpr const loggerbuf<level::fatal> fatal;
}
