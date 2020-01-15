/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "FactoryMock.h"

namespace Test::ZipDirFs::Containers::Helpers
{
	namespace Fixtures
	{
		FactoryMockDestructHelper* FactoryMockDestructHelper::instance;
		FactoryMockDestructHelper::FactoryMockDestructHelper() { instance = this; }
		FactoryMockDestructHelper::~FactoryMockDestructHelper() { instance = nullptr; }
		FactoryMock::FactoryMock() {}
		FactoryMock::~FactoryMock()
		{
			if (FactoryMockDestructHelper::instance != nullptr)
			{
				FactoryMockDestructHelper::instance->destroy(this);
			}
		}
	} // namespace Fixtures
} // namespace Test::ZipDirFs::Containers::Helpers
