/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "Factory.h"
#include "Fixtures/Lib.h"
#include "test/gtest.h"
#include <gmock/gmock.h>

namespace Test::ZipDirFs::Zip
{
	using ::ZipDirFs::Zip::Archive;
	using ::ZipDirFs::Zip::Factory;
	using BaseLib = ::ZipDirFs::Zip::Base::Lib;
	using ::testing::ByRef;
	using ::testing::Eq;
	using ::testing::Return;

	Factory*& FactoryAccess::getInstance() { return Factory::instance; }
	std::map<boost::filesystem::path, std::weak_ptr<Archive>>& FactoryAccess::getArchivesByPath()
	{
		return Factory::archivesByPath;
	}
	std::map<const ::ZipDirFs::Zip::Base::Lib*, std::weak_ptr<Archive>>&
		FactoryAccess::getArchivesByData()
	{
		return Factory::archivesByData;
	}
	FactoryOverride::FactoryOverride(Factory* temporary) : original(FactoryAccess::getInstance())
	{
		FactoryAccess::getInstance() = temporary;
	}
	FactoryOverride::~FactoryOverride() { FactoryAccess::getInstance() = original; }

	void FactoryTest::TearDown()
	{
		FactoryAccess::getArchivesByPath().clear();
		FactoryAccess::getArchivesByData().clear();
	}

	boost::filesystem::path FactoryTest::generatePath()
	{
		return boost::filesystem::path(std::string("root") + std::to_string(::Test::rand(UINT32_MAX)))
			.concat(std::string("inner") + std::to_string(::Test::rand(UINT32_MAX)));
	}

	TEST_F(FactoryTest, GloballyInstantiated) { EXPECT_NE(FactoryAccess::getInstance(), nullptr); }

	TEST_F(FactoryTest, GetInstance)
	{
		Factory* const factory = reinterpret_cast<decltype(factory)>(::Test::rand(UINT32_MAX));
		FactoryOverride over(factory);
		EXPECT_EQ(&Factory::getInstance(), factory);
	}

	TEST_F(FactoryTest, GetByPathCreate)
	{
		Fixtures::Lib lib;
		BaseLib* const opened = reinterpret_cast<decltype(opened)>(::Test::rand(UINT32_MAX));
		const boost::filesystem::path path(generatePath());
		EXPECT_CALL(lib, open(Eq(ByRef(path)))).WillOnce(Return(opened));
		EXPECT_CALL(lib, close(opened)); // Expected call by Archive destructor.
		auto archive(Factory::getInstance().get(path));
		ASSERT_NE(FactoryAccess::getArchivesByPath().find(path),
			FactoryAccess::getArchivesByPath().end());
		ASSERT_NE(FactoryAccess::getArchivesByData().find(opened),
			FactoryAccess::getArchivesByData().end());
		EXPECT_EQ(FactoryAccess::getArchivesByPath().find(path)->second.lock(), archive);
		EXPECT_EQ(FactoryAccess::getArchivesByData().find(opened)->second.lock(), archive);
	}

	TEST_F(FactoryTest, GetByPathUse)
	{
		Fixtures::Lib lib;
		const boost::filesystem::path path(generatePath());
		std::shared_ptr<Archive> archive(reinterpret_cast<Archive*>(::Test::rand(UINT32_MAX)), [](Archive*) {});
		FactoryAccess::getArchivesByPath().insert({path, archive});
		EXPECT_EQ(Factory::getInstance().get(path), archive);
	}

	TEST_F(FactoryTest, GetByPathExpired)
	{
		Fixtures::Lib lib;
		BaseLib* const opened = reinterpret_cast<decltype(opened)>(::Test::rand(UINT32_MAX));
		const boost::filesystem::path path(generatePath());
		EXPECT_CALL(lib, open(Eq(ByRef(path)))).WillOnce(Return(opened));
		EXPECT_CALL(lib, close(opened)); // Expected call by Archive destructor.
		std::shared_ptr<Archive> archive(reinterpret_cast<Archive*>(::Test::rand(UINT32_MAX)), [](Archive*) {});
		auto archivePtr(archive.get());
		FactoryAccess::getArchivesByPath().insert({path, archive});
		archive = nullptr;
		archive = Factory::getInstance().get(path);
		EXPECT_NE(archive.get(), archivePtr);
		ASSERT_NE(FactoryAccess::getArchivesByPath().find(path),
			FactoryAccess::getArchivesByPath().end());
		EXPECT_NE(FactoryAccess::getArchivesByPath().find(path)->second.lock().get(), archivePtr);
		EXPECT_EQ(FactoryAccess::getArchivesByPath().find(path)->second.lock(), archive);
	}

	TEST_F(FactoryTest, GetByDataAbsent)
	{
		BaseLib* const searched = reinterpret_cast<decltype(searched)>(::Test::rand(UINT32_MAX));
		EXPECT_EQ(Factory::getInstance().get(searched), nullptr);
	}

	TEST_F(FactoryTest, GetByDataUse)
	{
		BaseLib* const searched = reinterpret_cast<decltype(searched)>(::Test::rand(UINT32_MAX));
		std::shared_ptr<Archive> archive(reinterpret_cast<Archive*>(::Test::rand(UINT32_MAX)), [](Archive*) {});
		FactoryAccess::getArchivesByData().insert({searched, archive});
		EXPECT_EQ(Factory::getInstance().get(searched), archive);
	}

	TEST_F(FactoryTest, GetByDataExpired)
	{
		BaseLib* const searched = reinterpret_cast<decltype(searched)>(::Test::rand(UINT32_MAX));
		std::shared_ptr<Archive> archive(reinterpret_cast<Archive*>(::Test::rand(UINT32_MAX)), [](Archive*) {});
		FactoryAccess::getArchivesByData().insert({searched, archive});
		archive = nullptr;
		EXPECT_EQ(Factory::getInstance().get(searched), nullptr);
	}
} // namespace Test::ZipDirFs::Zip
