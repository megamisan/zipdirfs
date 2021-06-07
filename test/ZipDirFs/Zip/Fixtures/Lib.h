/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_ZIP_FIXTURES_LIB_H
#define TEST_ZIPDIRFS_ZIP_FIXTURES_LIB_H

#include "ZipDirFs/Zip/Base/Lib.h"
#include "ZipDirFs/Zip/Base/Stat.h"
#include <gmock/gmock.h>

namespace boost::filesystem
{
	struct path;
}

namespace Test::ZipDirFs::Zip
{
	namespace Fixtures
	{
		using namespace boost::filesystem;
		class Lib
		{
			using LibP = ::ZipDirFs::Zip::Base::Lib;
			using Stat = ::ZipDirFs::Zip::Base::Stat;

		public:
			Lib();
			~Lib();
			MOCK_METHOD1(open, LibP*(const path&));
			MOCK_METHOD1(close, void(LibP* const));
			MOCK_METHOD1(get_num_entries, std::uint64_t(LibP* const) noexcept);
			MOCK_METHOD2(stat, Stat(LibP* const, const std::string&));
			MOCK_METHOD2(stat_index, Stat(LibP* const, const std::uint64_t));
			MOCK_METHOD2(get_name, std::string(LibP* const, std::uint64_t));
			MOCK_METHOD2(fopen_index, LibP::File*(LibP* const, std::uint64_t));
			MOCK_METHOD3(fread, std::uint64_t(LibP::File* const, void*, std::uint64_t));
			MOCK_METHOD1(fclose, void(LibP::File* const));
		};

	} // namespace Fixtures
} // namespace Test::ZipDirFs::Zip

#endif // TEST_ZIPDIRFS_ZIP_FIXTURES_LIB_H
