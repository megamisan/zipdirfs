/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_ZIP_BASE_CONTENT_H
#define TEST_ZIPDIRFS_ZIP_BASE_CONTENT_H

#include "ZipDirFs/Zip/Base/Content.h"
#include <gtest/gtest.h>

namespace Test::ZipDirFs::Zip::Base
{
	struct ContentAccess : public ::ZipDirFs::Zip::Base::Content
	{
		static std::mutex& getRead(::ZipDirFs::Zip::Base::Content&);
		static std::mutex& getWrite(::ZipDirFs::Zip::Base::Content&);
		static std::condition_variable_any& getZeroReaders(::ZipDirFs::Zip::Base::Content&);
		static std::uint64_t& getReaderCount(::ZipDirFs::Zip::Base::Content&);
	};

} // namespace Test::ZipDirFs::Zip::Base

#endif // TEST_ZIPDIRFS_ZIP_BASE_CONTENT_H
