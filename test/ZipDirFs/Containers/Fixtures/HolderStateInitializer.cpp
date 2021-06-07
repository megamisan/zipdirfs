/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "HolderStateInitializer.h"

namespace Test::ZipDirFs::Containers
{
	namespace Fixtures
	{
		using WrapperBase = EntryIterator::Wrapper;

		HolderStateInitializer::HolderStateInitializer(const EntryGenerator& g) :
			accessor(g.testAccessor())
		{
		}
		std::map<off_t, EntryIterator>& HolderStateInitializer::offsetWrappers() const
		{
			return accessor.offsetWrappers();
		}
		std::set<WrapperBase*>& HolderStateInitializer::allWrappers() const
		{
			return accessor.allWrappers();
		}
		EntryIterator HolderStateInitializer::atCombined(
			EntryIterator&& base, EntryIterator&& pos) const
		{
			return accessor.atCombined(std::move(base), std::move(pos));
		}
		EntryIterator HolderStateInitializer::atDirect(EntryIterator&& pos) const
		{
			return accessor.atDirect(std::move(pos));
		}
	} // namespace Fixtures
} // namespace Test::ZipDirFs::Containers
