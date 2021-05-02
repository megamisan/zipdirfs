/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_FUSE_FIXTURES_FUNCTIONALENUMERATOR_H
#define TEST_ZIPDIRFS_FUSE_FIXTURES_FUNCTIONALENUMERATOR_H

#include "ZipDirFs/Containers/Helpers/Enumerator.h"
#include <string>
#include <functional>

namespace Test::ZipDirFs::Fuse
{
	namespace Fixtures
	{
		struct FunctionalEnumerator : public ::ZipDirFs::Containers::Helpers::Enumerator<std::string>
		{
			FunctionalEnumerator(std::function<void()> resetFunc, std::function<void()> nextFunc, std::function<bool()> validFunc, std::function<const std::string&()> currentFunc);
			void reset();
			void next();
			bool valid();
			const std::string& current();

		protected:
			std::function<void()> resetFunc;
			std::function<void()> nextFunc;
			std::function<bool()> validFunc;
			std::function<const std::string&()> currentFunc;
		};
	} // namespace Fixtures
} // namespace Test::ZipDirFs::Fuse

#endif // TEST_ZIPDIRFS_FUSE_FIXTURES_FUNCTIONALENUMERATOR_H
