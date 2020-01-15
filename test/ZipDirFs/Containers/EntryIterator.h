/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_CONTAINERS_ENTRYITERATOR_H
#define TEST_ZIPDIRFS_CONTAINERS_ENTRYITERATOR_H

#include "Fixtures/EntryIteratorWrapperMock.h"

namespace Test::ZipDirFs::Containers
{
	using ::ZipDirFs::Containers::EntryIterator;
	using WrapperMock = Fixtures::EntryIteratorWrapperMock;

	struct EntryIteratorAccess : public EntryIterator
	{
	public:
		static EntryIterator::pointer_wrapper& getWrapper(EntryIterator&);
	};

	class EntryIteratorTest : public ::testing::Test
	{
	protected:
		WrapperMock* getWrapper(EntryIterator&);
		void setWrapper(EntryIterator&, EntryIterator::Wrapper*);
	};
} // namespace Test::ZipDirFs::Containers

#endif // TEST_ZIPDIRFS_CONTAINERS_ENTRYITERATOR_H
