#pragma once

#include <cstdint>
#include <compare>
#include <chrono>
#include <concepts>

#include "core/gl.h"


enum class TimeUnit
{
	seconds, milliseconds, microseconds,

	millis = milliseconds,
	micros = microseconds
};


class Time
{
public:
	using Unit = TimeUnit;

private:
	using IntegerTimeUnit = std::int64_t;

	IntegerTimeUnit _micros = 0;

public:
	constexpr Time() noexcept = default;
	constexpr Time(const Time&) noexcept = default;
	constexpr Time(Time&&) noexcept = default;
	constexpr ~Time() noexcept = default;

	constexpr Time& operator= (const Time&) noexcept = default;
	constexpr Time& operator= (Time&&) noexcept = default;

	constexpr bool operator== (const Time&) const noexcept = default;
	constexpr auto operator<=> (const Time&) const noexcept = default;


	constexpr float toSeconds() const { return static_cast<float>(_micros) / 1000000.f; }
	constexpr std::int64_t toMilliseconds() const { return _micros / 1000; }
	constexpr std::int64_t toMicroseconds() const { return _micros; }

	template <Unit _Unit>
	constexpr auto to() const
	{
		if constexpr (_Unit == Unit::seconds)
			return toSeconds();
		else if constexpr (_Unit == Unit::milliseconds)
			return toMilliseconds();
		else
			return toMicroseconds();
	}

private:
	constexpr explicit Time(IntegerTimeUnit micros) : _micros(micros) {}

public:
	static constexpr Time zero() { return Time(); }

	static inline Time now()
	{
		return Time(std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now()).time_since_epoch().count());
	}

	template <std::integral _Ty>
	static constexpr Time seconds(_Ty amount) { return Time(static_cast<IntegerTimeUnit>(amount * 1000000LL)); }

	template <std::floating_point _Ty>
	static constexpr Time seconds(_Ty amount) { return Time(static_cast<IntegerTimeUnit>(amount * 1000000LL)); }


	template <std::integral _Ty>
	static constexpr Time milliseconds(_Ty amount) { return Time(static_cast<IntegerTimeUnit>(amount * 1000LL)); }


	template <std::integral _Ty>
	static constexpr Time microseconds(_Ty amount) { return Time(static_cast<IntegerTimeUnit>(amount)); }


	template <Unit _Unit, std::integral _Ty>
	static constexpr auto from(_Ty amount)
	{
		if constexpr (_Unit == Unit::seconds)
			return seconds<_Ty>(amount);
		else if constexpr (_Unit == Unit::milliseconds)
			return milliseconds<_Ty>(amount);
		else
			return microseconds<_Ty>(amount);
	}

	template <Unit _Unit, std::floating_point _Ty>
	static constexpr auto from(_Ty amount)
	{
		if constexpr (_Unit == Unit::seconds)
			return seconds<_Ty>(amount);
		else if constexpr (_Unit == Unit::milliseconds)
			return milliseconds(static_cast<IntegerTimeUnit>(amount));
		else
			return microseconds(static_cast<IntegerTimeUnit>(amount));
	}

public:
	friend constexpr Time operator+ (Time left, Time right) { return Time(left._micros + right._micros); }
	friend constexpr Time& operator+= (Time& left, Time right) { return left._micros += right._micros, left; }

	friend constexpr Time operator- (Time left, Time right) { return Time(left._micros - right._micros); }
	friend constexpr Time& operator-= (Time& left, Time right) { return left._micros -= right._micros, left; }

	template <std::floating_point _Ty> friend constexpr Time operator* (Time left, _Ty right) { return Time::seconds(static_cast<_Ty>(left.toSeconds()) * right); }
	template <std::integral _Ty> friend constexpr Time operator* (Time left, _Ty right) { return Time(left._micros * static_cast<IntegerTimeUnit>(right)); }
	template <std::floating_point _Ty> friend constexpr Time operator* (_Ty left, Time right) { return Time::seconds(static_cast<_Ty>(right.toSeconds()) * left); }
	template <std::integral _Ty> friend constexpr Time operator* (_Ty left, Time right) { return Time(right._micros * static_cast<IntegerTimeUnit>(left)); }
	friend constexpr Time operator* (Time left, Time right) { return Time(left._micros * right._micros); }
	friend constexpr Time& operator*= (Time& left, Time right) { return left._micros *= right._micros, left; }
	template <std::floating_point _Ty> friend constexpr Time& operator*= (Time& left, _Ty right) { return left = left * right, left; }
	template <std::integral _Ty> friend constexpr Time& operator*= (Time& left, _Ty right) { return left = left * right, left; }

	template <std::floating_point _Ty> friend constexpr Time operator/ (Time left, _Ty right) { return Time::seconds(static_cast<_Ty>(left.toSeconds()) / right); }
	template <std::integral _Ty> friend constexpr Time operator/ (Time left, _Ty right) { return Time(left._micros / static_cast<IntegerTimeUnit>(right)); }
	template <std::floating_point _Ty> friend constexpr Time operator/ (_Ty left, Time right) { return Time::seconds(static_cast<_Ty>(right.toSeconds()) / left); }
	template <std::integral _Ty> friend constexpr Time operator/ (_Ty left, Time right) { return Time(right._micros / static_cast<IntegerTimeUnit>(left)); }
	friend constexpr Time operator/ (Time left, Time right) { return Time(left._micros / right._micros); }
	friend constexpr Time& operator/= (Time& left, Time right) { return left._micros /= right._micros, left; }
	template <std::floating_point _Ty> friend constexpr Time& operator/= (Time& left, _Ty right) { return left = left / right, left; }
	template <std::integral _Ty> friend constexpr Time& operator/= (Time& left, _Ty right) { return left = left / right, left; }

	friend constexpr Time operator% (Time left, Time right) { return Time(left._micros % right._micros); }
	friend constexpr Time& operator%= (Time& left, Time right) { return left._micros %= right._micros, left; }
};



class Clock
{
private:
	Time _startTime;

public:
	inline Clock() noexcept : _startTime(Time::now()) {}
	inline Clock(const Clock&) noexcept = default;
	inline Clock(Clock&&) noexcept = default;
	inline ~Clock() noexcept = default;

	inline Clock& operator= (const Clock&) noexcept = default;
	inline Clock& operator= (Clock&&) noexcept = default;

	inline bool operator== (const Clock&) const noexcept = default;
	inline auto operator<=> (const Clock&) const noexcept = default;


	inline Time getStartTime() const { return _startTime; }

	inline Time getElapsedTime() const { return Time::now() - _startTime; }

	inline Time restart(Time startTime = Time::now())
	{
		Time elapsed = startTime - _startTime;
		_startTime = startTime;

		return elapsed;
	}
};
