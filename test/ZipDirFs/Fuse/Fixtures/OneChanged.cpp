/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "OneChanged.h"

namespace Test::ZipDirFs::Fuse
{
	namespace Fixtures
	{
		OneChanged::OneChanged(std::time_t target) : called(false), time(target) {}
		bool OneChanged::operator()()
		{
			bool changed = !called;
			called = true;
			return changed;
		}
		OneChanged::operator std::time_t() const { return called ? time : (std::time_t)0; }
	} // namespace Fixtures
} // namespace Test::ZipDirFs::Fuse
