/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_CONTAINERS_ENTRYGENERATOR_H
#define TEST_ZIPDIRFS_CONTAINERS_ENTRYGENERATOR_H

#include "Helpers/Fixtures/ChangedMock.h"
#include "Helpers/Fixtures/EntryListProxyMock.h"
#include "Helpers/Fixtures/EnumeratorMock.h"
#include "Helpers/Fixtures/FactoryMock.h"
#include "ZipDirFs/Containers/EntryIterator.h"
#include <gtest/gtest.h>

namespace Test::ZipDirFs::Containers
{
	using ::ZipDirFs::Containers::EntryIterator;
	using namespace Helpers::Fixtures;

	struct WrapperAccess : public EntryIterator::Wrapper
	{
	public:
		static bool invokeEquals(const EntryIterator::Wrapper&, const EntryIterator::Wrapper&);
	};

	struct EntryGeneratorTest : public ::testing::Test
	{
	protected:
		ChangedMock* changed;
		EntryListProxyMock* elp;
		EnumeratorMock* enumerator;
		FactoryMock* factory;
		void makeMocks();
	};
} // namespace Test::ZipDirFs::Containers

#endif // TEST_ZIPDIRFS_CONTAINERS_ENTRYGENERATOR_H
