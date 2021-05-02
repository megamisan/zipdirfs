/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "NullFactory.h"

namespace Test::ZipDirFs::Fuse
{
	namespace Fixtures
	{
		const NullFactory::mapped_type NullFactory::create(const key_type&) { return nullptr; }
		void NullFactory::destroy(const mapped_type&) {}
	} // namespace Fixtures
} // namespace Test::ZipDirFs::Fuse
