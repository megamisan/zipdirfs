/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_CONTAINERS_HELPERS_FIXTURES_ENUMERATORMOCK_H
#define TEST_ZIPDIRFS_CONTAINERS_HELPERS_FIXTURES_ENUMERATORMOCK_H

#include "ZipDirFs/Containers/Helpers/Enumerator.h"
#include <gmock/gmock.h>
#include <string>

namespace Test::ZipDirFs::Containers::Helpers
{
	namespace Fixtures
	{
		struct EnumeratorMock;

		struct EnumeratorMockDestructHelper
		{
			MOCK_METHOD1(destroy, void(EnumeratorMock*));
			static EnumeratorMockDestructHelper* instance;
			EnumeratorMockDestructHelper();
			~EnumeratorMockDestructHelper();
		};

		struct EnumeratorMock : public ::ZipDirFs::Containers::Helpers::Enumerator<std::string>
		{
			EnumeratorMock();
			~EnumeratorMock();
			MOCK_METHOD0(reset, void());
			MOCK_METHOD0(next, void());
			MOCK_METHOD0(valid, bool());
			MOCK_METHOD0(current, const std::string&());
		};
	} // namespace Fixtures
} // namespace Test::ZipDirFs::Containers::Helpers

#endif // TEST_ZIPDIRFS_CONTAINERS_HELPERS_FIXTURES_ENUMERATORMOCK_H
