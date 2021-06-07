/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "EntryIteratorWrapperMock.h"

namespace Test::ZipDirFs::Containers
{
	namespace Fixtures
	{
		EntryIteratorMockCreateHelper* EntryIteratorMockCreateHelper::instance = nullptr;
		EntryIteratorMockDestructHelper* EntryIteratorMockDestructHelper::instance = nullptr;
		EntryIteratorMockManagerHelper* EntryIteratorMockManagerHelper::instance = nullptr;

		EntryIteratorMockManagerHelper::EntryIteratorMockManagerHelper() { instance = this; }
		EntryIteratorMockManagerHelper::~EntryIteratorMockManagerHelper()
		{
			instance = nullptr;
			auto it = managed.begin();
			while (it != managed.end())
			{
				delete *it;
				++it;
			}
		}
		void EntryIteratorMockManagerHelper::add(EntryIteratorWrapperMock* w)
		{
			managed.push_back(w);
		}
		void EntryIteratorMockManagerHelper::remove(EntryIteratorWrapperMock* w)
		{
			auto it = managed.begin();
			while (it != managed.end())
			{
				if (*it == w)
					it = managed.erase(it);
				else
					++it;
			}
		}

		EntryIteratorMockCreateHelper::EntryIteratorMockCreateHelper() { instance = this; }
		EntryIteratorMockCreateHelper::~EntryIteratorMockCreateHelper() { instance = nullptr; }

		EntryIteratorMockDestructHelper::EntryIteratorMockDestructHelper() { instance = this; }
		EntryIteratorMockDestructHelper::~EntryIteratorMockDestructHelper() { instance = nullptr; }

		int EntryIteratorWrapperMock::nextId = 0;

		EntryIteratorWrapperMock::EntryIteratorWrapperMock()
		{
			id = nextId++;
			source = 0;
			if (EntryIteratorMockCreateHelper::instance != nullptr)
				EntryIteratorMockCreateHelper::instance->create();
			if (EntryIteratorMockManagerHelper::instance != nullptr)
				EntryIteratorMockManagerHelper::instance->add(this);
		}
		EntryIteratorWrapperMock::~EntryIteratorWrapperMock()
		{
			if (EntryIteratorMockManagerHelper::instance != nullptr)
				EntryIteratorMockManagerHelper::instance->remove(this);
			if (EntryIteratorMockDestructHelper::instance != nullptr)
				EntryIteratorMockDestructHelper::instance->destroy(this);
		}
	} // namespace Fixtures
} // namespace Test::ZipDirFs::Containers
