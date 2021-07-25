/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_CONTAINERS_FIXTURE_HOLDERSTATEINITIALIZER_H
#define TEST_ZIPDIRFS_CONTAINERS_FIXTURE_HOLDERSTATEINITIALIZER_H
#ifdef ZIPDIRFS_CONTAINERS_ENTRYGENERATOR_H
#	ifndef BUILD_TEST
#		error BUILD_TEST must be defined before including ZipDirFs/Containers/EntryGenerator.h
#	endif
#endif
#ifndef BUILD_TEST
#	define BUILD_TEST
#endif
#include "ZipDirFs/Containers/EntryGenerator.h"

namespace Test::ZipDirFs::Containers
{
	namespace Fixtures
	{
		using ::ZipDirFs::Containers::EntryGenerator;
		using ::ZipDirFs::Containers::EntryIterator;

		class HolderStateInitializer
		{
		public:
			HolderStateInitializer(const EntryGenerator&);
			std::map<off_t, EntryIterator>& offsetWrappers() const;
			std::set<EntryIterator::Wrapper*>& allWrappers() const;
			EntryIterator atCombined(EntryIterator&&, EntryIterator&&) const;
			EntryIterator atDirect(EntryIterator&&) const;

		protected:
		private:
			EntryGenerator::HolderAccessor accessor;
		};

	} // namespace Fixtures
} // namespace Test::ZipDirFs::Containers

#endif // TEST_ZIPDIRFS_CONTAINERS_FIXTURE_HOLDERSTATEINITIALIZER_H
