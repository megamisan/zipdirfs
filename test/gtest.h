/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_GTEST_H_INCLUDED
#define TEST_GTEST_H_INCLUDED

#include <cmath>
#include <cstdint>
#include <limits>
#include <ostream>
#include <type_traits>

namespace Test
{
	struct Generator
	{
		static double next();
	};
	extern Generator generator;

	inline double rand() { return Generator::next(); }

	long double rand(const long double min, const long double max);
	std::uintmax_t rand(const std::uintmax_t min, const std::uintmax_t max);
	std::intmax_t rand(const std::intmax_t min, const std::intmax_t max);

	template <typename _V, typename _Vi, typename _Vs>
	_V rand(const _Vi min, const _Vs max)
	{
		static_assert(std::is_arithmetic<_V>::value);
		static_assert(std::is_arithmetic<_Vi>::value);
		static_assert(std::is_arithmetic<_Vs>::value);
		return _V(std::numeric_limits<_V>::is_integer ? (std::numeric_limits<_V>::is_signed ?
							rand(std::intmax_t(min), std::intmax_t(max)) :
							rand(std::uintmax_t(min), std::uintmax_t(max))) :
														  rand((long double)min, (long double)max));
	}

	template <typename _V>
	inline _V rand(const _V min, const _V max)
	{
		static_assert(std::is_arithmetic<_V>::value);
		return rand<_V, _V, _V>(min, max);
	}

	template <typename _V>
	inline _V rand(const _V max)
	{
		static_assert(std::is_arithmetic<_V>::value);
		return max >= 0 ? rand<_V, _V, _V>(_V(0), max) : rand<_V, _V, _V>(max, _V(0));
	}
} // namespace Test

namespace boost
{
	namespace filesystem
	{
		struct path;
		void PrintTo(const boost::filesystem::path& path, std::ostream* os);
	} // namespace filesystem
} // namespace boost

#endif // TEST_GTEST_H_INCLUDED
