/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "FunctionalEnumerator.h"

namespace Test::ZipDirFs::Fuse
{
	namespace Fixtures
	{
		FunctionalEnumerator::FunctionalEnumerator(std::function<void()> resetFunc,
			std::function<void()> nextFunc, std::function<bool()> validFunc,
			std::function<const std::string&()> currentFunc) :
			resetFunc(resetFunc),
			nextFunc(nextFunc), validFunc(validFunc), currentFunc(currentFunc)
		{
		}
		void FunctionalEnumerator::reset() { resetFunc(); }
		void FunctionalEnumerator::next() { nextFunc(); }
		bool FunctionalEnumerator::valid() { return validFunc(); }
		const std::string& FunctionalEnumerator::current() { return currentFunc(); }
	} // namespace Fixtures
} // namespace Test::ZipDirFs::Fuse
