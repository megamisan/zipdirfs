/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_ZIP_BASE_CONTENT_H
#define TEST_ZIPDIRFS_ZIP_BASE_CONTENT_H

#include "ZipDirFs/Zip/Base/Content.h"
#include <gtest/gtest.h>

namespace Test::ZipDirFs::Zip::Base
{
	struct ContentAccess : public ::ZipDirFs::Zip::Base::Content
	{
		static std::mutex& getGlobal(Content&);
		static std::condition_variable_any& getReleased(Content&);
		static std::int32_t& getReadersActive(Content&);
		static std::uint32_t& getWritersWaiting(Content&);
	};

	struct ContentLockAccess : public ::ZipDirFs::Zip::Base::Content::lock
	{
		static ::ZipDirFs::Zip::Base::Content*& getContent(lock&);
		static bool& getWriter(lock&);
	};

} // namespace Test::ZipDirFs::Zip::Base

#endif // TEST_ZIPDIRFS_ZIP_BASE_CONTENT_H
