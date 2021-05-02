/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "gtest_helper.h"
#include <cstdlib>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace Test
{
	std::vector<std::unique_ptr<HelperInfo>> helpers;
	std::map<std::string, runner_func> helpersByName;

	const HelperInfo* CreateAndRegisterHelper(const char* name, const runner_func&& runner)
	{
		HelperInfo* helper = new HelperInfo{name, runner};
		helpers.push_back(std::move(std::unique_ptr<HelperInfo>(helper)));
		helpersByName[name] = runner;
		return helper;
	}
} // namespace Test

int main(int argc, char** argv)
{
	auto it = ::Test::helpersByName.find(argv[0]);
	if (it == ::Test::helpersByName.end() && argc > 1)
	{
		it = ::Test::helpersByName.find(argv[1]);
		argc--;
		argv++;
	}
	if (!(it == ::Test::helpersByName.end()))
	{
		return it->second(argc, argv);
	}
	std::cerr << "Helper not defined." << std::endl;
	return 1;
}
