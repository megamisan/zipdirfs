/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ChangedMock.h"

namespace Test::ZipDirFs::Containers::Helpers
{
	namespace Fixtures
	{
		ChangedMockDestructHelper* ChangedMockDestructHelper::instance;
		ChangedMockDestructHelper::ChangedMockDestructHelper() { instance = this; }
		ChangedMockDestructHelper::~ChangedMockDestructHelper() { instance = nullptr; }
		ChangedMock::ChangedMock() {}
		ChangedMock::~ChangedMock()
		{
			if (ChangedMockDestructHelper::instance != nullptr)
			{
				ChangedMockDestructHelper::instance->destroy(this);
			}
		}
		bool ChangedMock::operator()() { return invoke(); }
	} // namespace Fixtures
} // namespace Test::ZipDirFs::Containers::Helpers
