/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_CONTAINERS_FIXTURES_ENTRYITERATORENDWRAPPER_H
#define TEST_ZIPDIRFS_CONTAINERS_FIXTURES_ENTRYITERATORENDWRAPPER_H

#include "ZipDirFs/Containers/EntryIterator.h"
#include <gmock/gmock.h>
#include <vector>

namespace Test::ZipDirFs::Containers
{
	namespace Fixtures
	{
		using EntryIteratorWrapperBase = ::ZipDirFs::Containers::EntryIterator::Wrapper;

		struct EntryIteratorEndWrapper : public EntryIteratorWrapperBase
		{
			EntryIteratorEndWrapper(const std::int64_t);
			~EntryIteratorEndWrapper();
			EntryIteratorWrapperBase* clone() const;
			reference dereference() const;
			void increment();
			bool equals(const EntryIteratorWrapperBase& w) const;
			operator const std::int64_t() const;
		protected:
			const std::int64_t value;
		};
	} // namespace Fixtures
} // namespace Test::ZipDirFs::Containers

#endif // TEST_ZIPDIRFS_CONTAINERS_FIXTURES_ENTRYITERATORENDWRAPPER_H
