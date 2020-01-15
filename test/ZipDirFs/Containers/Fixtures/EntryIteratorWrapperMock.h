/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_CONTAINERS_FIXTURES_ENTRYITERATORWRAPPERMOCK_H
#define TEST_ZIPDIRFS_CONTAINERS_FIXTURES_ENTRYITERATORWRAPPERMOCK_H

#include "ZipDirFs/Containers/EntryIterator.h"
#include <gmock/gmock.h>
#include <vector>

namespace Test::ZipDirFs::Containers
{
	namespace Fixtures
	{
		using EntryIteratorWrapperBase = ::ZipDirFs::Containers::EntryIterator::Wrapper;

		struct EntryIteratorWrapperMock;

		struct EntryIteratorMockManagerHelper
		{
			static EntryIteratorMockManagerHelper* instance;
			EntryIteratorMockManagerHelper();
			~EntryIteratorMockManagerHelper();
			void add(EntryIteratorWrapperMock* w);
			void remove(EntryIteratorWrapperMock* w);

		private:
			std::vector<EntryIteratorWrapperMock*> managed;
		};

		struct EntryIteratorMockCreateHelper
		{
			MOCK_METHOD0(create, void());
			static EntryIteratorMockCreateHelper* instance;
			EntryIteratorMockCreateHelper();
			~EntryIteratorMockCreateHelper();
		};

		struct EntryIteratorMockDestructHelper
		{
			MOCK_METHOD1(destroy, void(const EntryIteratorWrapperMock*));
			static EntryIteratorMockDestructHelper* instance;
			EntryIteratorMockDestructHelper();
			~EntryIteratorMockDestructHelper();
		};

		struct EntryIteratorWrapperMock : public EntryIteratorWrapperBase
		{
			static int nextId;
			int id, source;
			EntryIteratorWrapperMock();
			~EntryIteratorWrapperMock();
			MOCK_CONST_METHOD0(clone, EntryIteratorWrapperBase*());
			MOCK_CONST_METHOD0(dereference, reference());
			MOCK_METHOD0(increment, void());
			bool equals(const EntryIteratorWrapperBase& w) const { return equals_proxy(&w); }
			MOCK_CONST_METHOD1(equals_proxy, bool(const EntryIteratorWrapperBase*));
		};
	} // namespace Fixtures
} // namespace Test::ZipDirFs::Containers

#endif // TEST_ZIPDIRFS_CONTAINERS_FIXTURES_ENTRYITERATORWRAPPERMOCK_H
