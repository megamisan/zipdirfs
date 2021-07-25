/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_CONTAINERS_HELPERS_FIXTURES_FACTORYMOCK_H
#define TEST_ZIPDIRFS_CONTAINERS_HELPERS_FIXTURES_FACTORYMOCK_H

#include "ZipDirFs/Containers/Helpers/Factory.h"
#include <gmock/gmock.h>

namespace Test::ZipDirFs::Containers::Helpers
{
	namespace Fixtures
	{
		struct FactoryMock;

		struct FactoryMockDestructHelper
		{
			MOCK_METHOD1(destroy, void(FactoryMock*));
			static FactoryMockDestructHelper* instance;
			FactoryMockDestructHelper();
			~FactoryMockDestructHelper();
		};

		struct FactoryMock : public ::ZipDirFs::Containers::Helpers::Factory
		{
			FactoryMock();
			~FactoryMock();
			MOCK_METHOD1(create, const mapped_type(const key_type&));
			MOCK_METHOD1(destroy, void(const mapped_type&));
		};
	} // namespace Fixtures
} // namespace Test::ZipDirFs::Containers::Helpers

#endif // TEST_ZIPDIRFS_CONTAINERS_HELPERS_FIXTURES_FACTORYMOCK_H
