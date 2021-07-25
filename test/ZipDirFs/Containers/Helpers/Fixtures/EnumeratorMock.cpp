/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "EnumeratorMock.h"

namespace Test::ZipDirFs::Containers::Helpers
{
	namespace Fixtures
	{
		EnumeratorMockDestructHelper* EnumeratorMockDestructHelper::instance;
		EnumeratorMockDestructHelper::EnumeratorMockDestructHelper() { instance = this; }
		EnumeratorMockDestructHelper::~EnumeratorMockDestructHelper() { instance = nullptr; }
		EnumeratorMock::EnumeratorMock() {}
		EnumeratorMock::~EnumeratorMock()
		{
			if (EnumeratorMockDestructHelper::instance != nullptr)
			{
				EnumeratorMockDestructHelper::instance->destroy(this);
			}
		}
	} // namespace Fixtures
} // namespace Test::ZipDirFs::Containers::Helpers
