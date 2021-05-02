/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_GTEST_HELPER_H_INCLUDED
#define TEST_GTEST_HELPER_H_INCLUDED

#include <string>
#include <functional>

namespace Test
{
	typedef std::function<int(int argc, char** argv)> runner_func;

	struct HelperInfo
	{
		const char* name;
		runner_func runner;
	};

	const HelperInfo* CreateAndRegisterHelper(const char* name, const runner_func&& runner);
}

#define TEST_HELPER_CLASS_NAME_(name) Helper##name
#define TEST_HELPER(name,runner) \
	const ::Test::HelperInfo* const TEST_HELPER_CLASS_NAME_(name) = \
	::Test::CreateAndRegisterHelper(#name, std::move(runner))

#endif // TEST_GTEST_HELPER_H_INCLUDED
