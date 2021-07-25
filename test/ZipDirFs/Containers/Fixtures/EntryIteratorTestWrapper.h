/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_CONTAINERS_FIXTURES_ENTRYITERATORTESTWRAPPER_H
#define TEST_ZIPDIRFS_CONTAINERS_FIXTURES_ENTRYITERATORTESTWRAPPER_H

#include "ZipDirFs/Containers/EntryIterator.h"
#include <gmock/gmock.h>
#include <string>
#include <vector>

namespace Test::ZipDirFs::Containers
{
	namespace Fixtures
	{
		using EntryIteratorWrapperBase = ::ZipDirFs::Containers::EntryIterator::Wrapper;

		struct EntryIteratorTestWrapper : public EntryIteratorWrapperBase
		{
			typedef std::vector<std::string> list_type;
			EntryIteratorTestWrapper(list_type::iterator);
			~EntryIteratorTestWrapper();
			EntryIteratorWrapperBase* clone() const;
			reference dereference() const;
			void increment();
			bool equals(const EntryIteratorWrapperBase& w) const;

		protected:
			list_type::iterator value;
		};
	} // namespace Fixtures
} // namespace Test::ZipDirFs::Containers

#endif // TEST_ZIPDIRFS_CONTAINERS_FIXTURES_ENTRYITERATORTESTWRAPPER_H
