#pragma once

#include <algorithm>
#include <concepts>
#include <cmath>


using radians_t = double;
using degrees_t = double;


namespace utils
{
	constexpr double pi = 3.14159265358979323846;
	constexpr double mid_pi = pi / 2;
	constexpr double max_degrees = 360;
	constexpr double deg_to_rad = pi / (max_degrees / 2);
	constexpr double rad_to_deg = (max_degrees / 2) / pi;


	constexpr radians_t radians(degrees_t deg) { return radians_t(deg * deg_to_rad); }
	constexpr degrees_t degrees(radians_t rad) { return degrees_t(rad * rad_to_deg); }



	template <typename _Ty>
	concept numerical = std::integral<_Ty> || std::floating_point<_Ty>;

	template <typename _Ty, typename _NumTy>
	concept dim2vec = numerical<_NumTy> && requires(_Ty n)
	{
		{ n.x } -> std::convertible_to<_NumTy>;
		{ n.y } -> std::convertible_to<_NumTy>;
	};

	template <typename _Ty, typename _NumTy>
	concept dim3vec = numerical<_NumTy> && requires(_Ty n)
	{
		{ n.x } -> std::convertible_to<_NumTy>;
		{ n.y } -> std::convertible_to<_NumTy>;
		{ n.z } -> std::convertible_to<_NumTy>;
	};

	template <typename _Ty, typename _NumTy>
	concept dim4vec = numerical<_NumTy> && requires(_Ty n)
	{
		{ n.x } -> std::convertible_to<_NumTy>;
		{ n.y } -> std::convertible_to<_NumTy>;
		{ n.z } -> std::convertible_to<_NumTy>;
		{ n.w } -> std::convertible_to<_NumTy>;
	};
}
