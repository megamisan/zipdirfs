/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_CONTAINERS_FIXTURES_HELPERS_H_INCLUDED
#define TEST_ZIPDIRFS_CONTAINERS_FIXTURES_HELPERS_H_INCLUDED

#include <gmock/gmock.h>
#include <string>

namespace std
{
	bool operator==(const string& it, const ::testing::internal::ReferenceOrValueWrapper<string>& wrap);
	bool operator==(
		const string& it, const ::testing::internal::ReferenceOrValueWrapper<const string>& wrap);
} // namespace std
#endif // TEST_ZIPDIRFS_CONTAINERS_FIXTURES_HELPERS_H_INCLUDED
