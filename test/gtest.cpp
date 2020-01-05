/*
 * Copyright © 2019-2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "gtest.h"
#include <boost/filesystem.hpp>
#include <ctime>
#include <ostream>
#include <random>

namespace
{
	unsigned init();
	unsigned gtest_rand_init = []() {
		unsigned seed = std::time(nullptr);
		std::srand(seed);
		return seed;
	}();
} // namespace

namespace Test
{
	Generator generator;
	namespace
	{
		std::default_random_engine randgen((std::random_device())());
		std::uniform_real_distribution<double> distribution;
	} // namespace
	double Generator::next() { return distribution(randgen); }
	long double rand(const long double min, const long double max)
	{
		return min
			+ Generator::next()
			* std::nextafter(max - min, std::numeric_limits<long double>::max());
	}
	std::uintmax_t rand(const std::uintmax_t min, const std::uintmax_t max)
	{
		const auto limit = std::numeric_limits<std::uintmax_t>::max();
		const auto size = max - min;
		return min + Generator::next() * (size == limit ? limit : (size + 1));
	}
	std::intmax_t rand(const std::intmax_t min, const std::intmax_t max)
	{
		const auto limit = std::numeric_limits<std::intmax_t>::max();
		const auto size = max - min;
		return min + Generator::next() * (size == limit ? limit : (size + 1));
	}
} // namespace Test

namespace boost
{
	namespace filesystem
	{
		void PrintTo(const boost::filesystem::path& path, std::ostream* os)
		{
			*os << path;
		}
	} // namespace filesystem
} // namespace boost
