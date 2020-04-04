/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipFactory.h"
#include "../../fusekit/Fixtures/EntryMock.h"
#include "../Utilities/Fixtures/FileSystem.h"
#include "../Zip/Fixtures/Lib.h"
#include "Fixtures/LibInstance.h"
#include "ZipDirFs/Components/ZipFactory.h"
#include "test/gtest.h"
#include <boost/filesystem.hpp>
#include <gtest/gtest.h>

namespace Test::ZipDirFs::Components
{
	using namespace boost::filesystem;
	using ::Test::ZipDirFs::Utilities::Fixtures::FileSystem;
	using ::Test::ZipDirFs::Zip::Fixtures::Lib;
	using ::testing::_;
	using ::testing::ByRef;
	using ::testing::Eq;
	using ::testing::Return;
	using ::testing::Throw;

	namespace
	{
		using ::Test::fusekit::Fixtures::EntryMock;
		using ::ZipDirFs::Components::ZipFactory;
		using ::ZipDirFs::Containers::Helpers::Changed;

		struct NullChanged : public Changed
		{
			bool operator()() { return false; };
			operator std::time_t() const { return 0; };
		};

		struct ZipDirectoryEntryMock : EntryMock
		{
			ZipDirectoryEntryMock(const path& p, const path& i, std::shared_ptr<Changed>& c) :
				path(p), inner(i), changed(c){};
			const struct path path;
			const struct path inner;
			std::shared_ptr<Changed> changed;
		};

		struct ZipFileEntryMock : EntryMock
		{
			ZipFileEntryMock(const path& p, const path& i, std::shared_ptr<Changed>& c) :
				path(p), inner(i), changed(c){};
			const struct path path;
			const struct path inner;
			std::shared_ptr<Changed> changed;
		};

		typedef ZipFactory<ZipDirectoryEntryMock, ZipFileEntryMock> TestedFactory;
	} // namespace

	TEST(ZipFactory, Directory)
	{
		Lib lib;
		Fixtures::LibInstance libInstance;
		const std::string inner(std::to_string(::Test::rand(UINT32_MAX))),
			item(std::to_string(::Test::rand(UINT32_MAX))),
			expected(inner + "/" + item + "/");
		std::shared_ptr<Changed> changed(new NullChanged());
		const path parent(path("path") / std::to_string(::Test::rand(UINT32_MAX)));
		EXPECT_CALL(lib, open(parent)).WillOnce(Return(&libInstance));
		EXPECT_CALL(lib, get_num_entries(&libInstance)).WillOnce(Return(1));
		EXPECT_CALL(lib, get_name(&libInstance, 0))
			.WillOnce(Return(expected + std::to_string(::Test::rand(UINT32_MAX))));
		EXPECT_CALL(lib, close(&libInstance));
		TestedFactory factory(parent, inner + "/", changed);
		std::unique_ptr<::fusekit::entry, std::function<void(::fusekit::entry*)>> created(
			factory.create(item), [&factory](::fusekit::entry* p) { factory.destroy(p); });
		auto result = dynamic_cast<ZipDirectoryEntryMock*>(created.get());
		ASSERT_NE(result, nullptr);
		ASSERT_EQ(result->path, parent);
		ASSERT_EQ(result->inner, expected);
		ASSERT_EQ(result->changed, changed);
	}

	TEST(ZipFactory, FileRoot)
	{
		Lib lib;
		Fixtures::LibInstance libInstance;
		const std::string item(std::to_string(::Test::rand(UINT32_MAX)));
		const path parent(path("path") / std::to_string(::Test::rand(UINT32_MAX)));
		std::shared_ptr<Changed> changed(new NullChanged());
		EXPECT_CALL(lib, open(parent)).WillOnce(Return(&libInstance));
		EXPECT_CALL(lib, get_num_entries(&libInstance)).WillOnce(Return(1));
		EXPECT_CALL(lib, get_name(&libInstance, 0)).WillOnce(Return(item));
		EXPECT_CALL(lib, close(&libInstance));
		TestedFactory factory(parent, "", changed);
		std::unique_ptr<::fusekit::entry, std::function<void(::fusekit::entry*)>> created(
			factory.create(item), [&factory](::fusekit::entry* p) { factory.destroy(p); });
		auto result = dynamic_cast<ZipFileEntryMock*>(created.get());
		ASSERT_NE(result, nullptr);
		ASSERT_EQ(result->path, parent);
		ASSERT_EQ(result->inner, item);
		ASSERT_EQ(result->changed, changed);
	}

	TEST(ZipFactory, FileInner)
	{
		Lib lib;
		Fixtures::LibInstance libInstance;
		const std::string inner(std::to_string(::Test::rand(UINT32_MAX)) + "/");
		const std::string item(std::to_string(::Test::rand(UINT32_MAX)));
		std::shared_ptr<Changed> changed(new NullChanged());
		const path parent(path("path") / std::to_string(::Test::rand(UINT32_MAX)));
		EXPECT_CALL(lib, open(parent)).WillOnce(Return(&libInstance));
		EXPECT_CALL(lib, get_num_entries(&libInstance)).WillOnce(Return(1));
		EXPECT_CALL(lib, get_name(&libInstance, 0)).WillOnce(Return(inner + item));
		EXPECT_CALL(lib, close(&libInstance));
		TestedFactory factory(parent, inner, changed);
		std::unique_ptr<::fusekit::entry, std::function<void(::fusekit::entry*)>> created(
			factory.create(item), [&factory](::fusekit::entry* p) { factory.destroy(p); });
		auto result = dynamic_cast<ZipFileEntryMock*>(created.get());
		ASSERT_NE(result, nullptr);
		ASSERT_EQ(result->path, parent);
		ASSERT_EQ(result->inner, inner + item);
		ASSERT_EQ(result->changed, changed);
	}

} // namespace Test::ZipDirFs::Components
