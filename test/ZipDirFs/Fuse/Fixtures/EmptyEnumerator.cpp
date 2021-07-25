/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "EmptyEnumerator.h"

namespace Test::ZipDirFs::Fuse
{
	namespace Fixtures
	{
		void EmptyEnumerator::reset() {}
		void EmptyEnumerator::next() {}
		bool EmptyEnumerator::valid() { return false; }
		const std::string& EmptyEnumerator::current() { throw "Empty enumerator"; }
	} // namespace Fixtures
} // namespace Test::ZipDirFs::Fuse
