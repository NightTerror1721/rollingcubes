#pragma once

#include <string>
#include <sstream>
#include <vector>

#include "utf.h"


template <typename _Ty>
concept UtfChar =
	std::same_as<char, _Ty> ||
	std::same_as<char8_t, _Ty> ||
	std::same_as<char16_t, _Ty> ||
	std::same_as<char32_t, _Ty>;


class UnicodeString : public std::u32string
{
private:
	template <typename _CharTy> struct UtfClass;
	template <> struct UtfClass<char> { using utf = Utf8; };
	template <> struct UtfClass<char8_t> { using utf = Utf8; };
	template <> struct UtfClass<char16_t> { using utf = Utf16; };
	template <> struct UtfClass<char32_t> { using utf = Utf32; };

public:
	UnicodeString() = default;
	UnicodeString(const UnicodeString&) = default;
	UnicodeString(UnicodeString&&) noexcept = default;
	~UnicodeString() = default;

	UnicodeString& operator= (const UnicodeString&) = default;
	UnicodeString& operator= (UnicodeString&&) noexcept = default;

	bool operator== (const UnicodeString&) const = default;
	auto operator<=> (const UnicodeString&) const = default;

public:
	template <UtfChar _CharTy>
	inline UnicodeString(const std::basic_string<_CharTy>& str) : std::u32string(toUtf32<_CharTy>(str)) {}

	template <UtfChar _CharTy>
	inline UnicodeString(std::basic_string_view<_CharTy> str) : std::u32string(toUtf32<_CharTy>(str)) {}

	template <UtfChar _CharTy>
	inline UnicodeString(const _CharTy* str) : std::u32string(toUtf32<_CharTy>(str)) {}

	template <UtfChar _CharTy>
	inline UnicodeString(const _CharTy* str, std::size_t count) : std::u32string(toUtf32<_CharTy>(std::basic_string_view<_CharTy>(str, count))) {}

private:
	template <UtfChar _CharTy>
	static std::u32string toUtf32(std::basic_string_view<_CharTy> str);
};


template <>
inline std::u32string UnicodeString::toUtf32<char32_t>(std::basic_string_view<char32_t> str)
{
	return std::u32string(str);
}

template <>
inline std::u32string UnicodeString::toUtf32<char>(std::basic_string_view<char> str)
{
	auto it = str.data();
	const auto sentinel = it + str.size();

	std::vector<char32_t> data;
	data.resize(str.size());

	Utf32::fromAnsi(it, sentinel, data.data());

	return std::u32string(data.data(), data.size());
}

template <UtfChar _CharTy>
inline std::u32string UnicodeString::toUtf32(std::basic_string_view<_CharTy> str)
{
	auto it = str.data();
	const auto sentinel = it + str.size();

	std::vector<char32_t> data;
	data.reserve(str.size());

	while (it < sentinel)
	{
		std::uint32_t result;
		it = UtfClass<_CharTy>::utf::decode(it, sentinel, result);
		if (result != 0)
			data.push_back(static_cast<char32_t>(result));
	}

	return std::u32string(data.data(), data.size());
}
