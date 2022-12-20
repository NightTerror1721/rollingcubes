#pragma once

#include <exception>
#include <concepts>

#include "string_utils.h"


namespace utils
{
	template <typename _Ty = std::exception, typename... _ArgsTys> requires
		std::derived_from<_Ty, std::exception> &&
		std::constructible_from<_Ty, const char*>
	inline _Ty formatException(std::string_view fmt, _ArgsTys&&... args)
	{
		return _Ty(utils::str::format(fmt, std::forward<_ArgsTys>(args)...).c_str());
	}

	template <typename _Ty = std::exception, typename... _ArgsTys> requires
		std::derived_from<_Ty, std::exception>&&
		std::constructible_from<_Ty, const char*>
	inline void throwFormattedException(std::string_view fmt, _ArgsTys&&... args)
	{
		throw formatException<_Ty>(fmt, std::forward<_ArgsTys>(args)...);
	}
}
