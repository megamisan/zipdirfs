/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_CONTAINERS_HELPERS_ENTRYLISTPROXYMOCK_H
#define TEST_ZIPDIRFS_CONTAINERS_HELPERS_ENTRYLISTPROXYMOCK_H

#include "ZipDirFs/Containers/Helpers/EntryListProxy.h"
#include <gmock/gmock.h>

namespace Test::ZipDirFs::Containers::Helpers
{
	namespace Fixtures
	{
		struct EntryListProxyMock;

		struct EntryListProxyMockDestructHelper
		{
			MOCK_METHOD1(destroy, void(EntryListProxyMock*));
			static EntryListProxyMockDestructHelper* instance;
			EntryListProxyMockDestructHelper();
			~EntryListProxyMockDestructHelper();
		};

		struct EntryListProxyMock : public ::ZipDirFs::Containers::Helpers::EntryListProxy
		{
			EntryListProxyMock();
			~EntryListProxyMock();
			MOCK_METHOD0(begin, iterator());
			MOCK_METHOD0(end, iterator());
			iterator insert(iterator it, const key_type& k, const mapped_type& m) { return insert_proxy(it, k, m); }
			MOCK_METHOD3(insert_proxy, iterator(const iterator&, const key_type&, const mapped_type&));
			iterator erase(iterator it) { return erase_proxy(it); }
			MOCK_METHOD1(erase_proxy, iterator(const iterator&));
			MOCK_CONST_METHOD1(find, fusekit::entry*(const key_type&));
		};
	} // namespace Fixtures
} // namespace Test::ZipDirFs::Containers::Helpers

#endif // TEST_ZIPDIRFS_CONTAINERS_HELPERS_ENTRYLISTPROXYMOCK_H
