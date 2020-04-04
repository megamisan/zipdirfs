/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_CONTAINERS_HELPERS_FIXTURES_CHANGEDMOCK_H
#define TEST_ZIPDIRFS_CONTAINERS_HELPERS_FIXTURES_CHANGEDMOCK_H

#include "ZipDirFs/Containers/Helpers/Changed.h"
#include <gmock/gmock.h>

namespace Test::ZipDirFs::Containers::Helpers
{
	namespace Fixtures
	{
		struct ChangedMock;

		struct ChangedMockDestructHelper
		{
			MOCK_METHOD1(destroy, void(ChangedMock*));
			static ChangedMockDestructHelper* instance;
			ChangedMockDestructHelper();
			~ChangedMockDestructHelper();
		};

		struct ChangedMock : public ::ZipDirFs::Containers::Helpers::Changed
		{
			ChangedMock();
			~ChangedMock();
			bool operator()();
			MOCK_METHOD0(invoke, bool());
			operator std::time_t() const;
			MOCK_CONST_METHOD0(get, std::time_t());
		};
	} // namespace Fixtures
} // namespace Test::ZipDirFs::Containers::Helpers

#endif // TEST_ZIPDIRFS_CONTAINERS_HELPERS_FIXTURES_CHANGEDMOCK_H
