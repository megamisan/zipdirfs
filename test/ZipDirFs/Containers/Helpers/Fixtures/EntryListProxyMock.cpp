/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "EntryListProxyMock.h"

namespace Test::ZipDirFs::Containers::Helpers
{
	namespace Fixtures
	{
		EntryListProxyMockDestructHelper* EntryListProxyMockDestructHelper::instance;
		EntryListProxyMockDestructHelper::EntryListProxyMockDestructHelper() { instance = this; }
		EntryListProxyMockDestructHelper::~EntryListProxyMockDestructHelper()
		{
			instance = nullptr;
		}
		EntryListProxyMock::EntryListProxyMock() {}
		EntryListProxyMock::~EntryListProxyMock()
		{
			if (EntryListProxyMockDestructHelper::instance != nullptr)
			{
				EntryListProxyMockDestructHelper::instance->destroy(this);
			}
		}
	} // namespace Fixtures
} // namespace Test::ZipDirFs::Containers::Helpers
