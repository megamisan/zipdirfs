/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "EntryProxy.h"
#include "../../fusekit/Fixtures/EntryMock.h"
#include "ZipDirFs/Fuse/EntryProxy.h"
#include "test/gtest.h"
#include <gtest/gtest.h>

namespace Test::ZipDirFs::Fuse
{
	using ::Test::fusekit::Fixtures::EntryMock;
	using ::testing::Eq;
	using ::testing::Ref;
	using ::testing::Return;
	using ::ZipDirFs::Fuse::EntryProxy;

	TEST(EntryProxyTest, child)
	{
		std::unique_ptr<EntryMock> mock(new EntryMock());
		std::string name("item" + std::to_string(::Test::rand(UINT32_MAX)));
		::fusekit::entry* res = reinterpret_cast<::fusekit::entry*>(::Test::rand(UINT32_MAX));
		EXPECT_CALL(*mock, child(name.c_str())).WillOnce(Return(res));
		EntryProxy proxy;
		proxy = std::move(mock);
		EXPECT_EQ(proxy.child(name.c_str()), res);
	}
	TEST(EntryProxyTest, stat)
	{
		std::unique_ptr<EntryMock> mock(new EntryMock());
		struct stat stbuf;
		int res(::Test::rand(UINT32_MAX));
		EXPECT_CALL(*mock, stat(Ref(stbuf))).WillOnce(Return(res));
		EntryProxy proxy;
		proxy = std::move(mock);
		EXPECT_EQ(proxy.stat(stbuf), res);
	}
	TEST(EntryProxyTest, access)
	{
		std::unique_ptr<EntryMock> mock(new EntryMock());
		int mode(::Test::rand(UINT32_MAX)), res(::Test::rand(UINT32_MAX));
		EXPECT_CALL(*mock, access(mode)).WillOnce(Return(res));
		EntryProxy proxy;
		proxy = std::move(mock);
		EXPECT_EQ(proxy.access(mode), res);
	}
	TEST(EntryProxyTest, chmod)
	{
		std::unique_ptr<EntryMock> mock(new EntryMock());
		mode_t mode(::Test::rand(UINT32_MAX));
		int res(::Test::rand(UINT32_MAX));
		EXPECT_CALL(*mock, chmod(mode)).WillOnce(Return(res));
		EntryProxy proxy;
		proxy = std::move(mock);
		EXPECT_EQ(proxy.chmod(mode), res);
	}
	TEST(EntryProxyTest, open)
	{
		std::unique_ptr<EntryMock> mock(new EntryMock());
		fuse_file_info fi;
		int res(::Test::rand(UINT32_MAX));
		EXPECT_CALL(*mock, open(Ref(fi))).WillOnce(Return(res));
		EntryProxy proxy;
		proxy = std::move(mock);
		EXPECT_EQ(proxy.open(fi), res);
	}
	TEST(EntryProxyTest, release)
	{
		std::unique_ptr<EntryMock> mock(new EntryMock());
		fuse_file_info fi;
		int res(::Test::rand(UINT32_MAX));
		EXPECT_CALL(*mock, release(Ref(fi))).WillOnce(Return(res));
		EntryProxy proxy;
		proxy = std::move(mock);
		EXPECT_EQ(proxy.release(fi), res);
	}
	TEST(EntryProxyTest, read)
	{
		std::unique_ptr<EntryMock> mock(new EntryMock());
		char* buf(reinterpret_cast<char*>(::Test::rand(UINT32_MAX)));
		size_t len(::Test::rand(UINT32_MAX));
		off_t off(::Test::rand(INT32_MAX));
		fuse_file_info fi;
		int res(::Test::rand(UINT32_MAX));
		EXPECT_CALL(*mock, read(buf, len, off, Ref(fi))).WillOnce(Return(res));
		EntryProxy proxy;
		proxy = std::move(mock);
		EXPECT_EQ(proxy.read(buf, len, off, fi), res);
	}
	TEST(EntryProxyTest, write)
	{
		std::unique_ptr<EntryMock> mock(new EntryMock());
		const char* buf(reinterpret_cast<const char*>(::Test::rand(UINT32_MAX)));
		size_t len(::Test::rand(UINT32_MAX));
		off_t off(::Test::rand(INT32_MAX));
		fuse_file_info fi;
		int res(::Test::rand(UINT32_MAX));
		EXPECT_CALL(*mock, write(buf, len, off, Ref(fi))).WillOnce(Return(res));
		EntryProxy proxy;
		proxy = std::move(mock);
		EXPECT_EQ(proxy.write(buf, len, off, fi), res);
	}
	TEST(EntryProxyTest, opendir)
	{
		std::unique_ptr<EntryMock> mock(new EntryMock());
		fuse_file_info fi;
		int res(::Test::rand(UINT32_MAX));
		EXPECT_CALL(*mock, opendir(Ref(fi))).WillOnce(Return(res));
		EntryProxy proxy;
		proxy = std::move(mock);
		EXPECT_EQ(proxy.opendir(fi), res);
	}
	TEST(EntryProxyTest, readdir)
	{
		std::unique_ptr<EntryMock> mock(new EntryMock());
		void* buf(reinterpret_cast<void*>(::Test::rand(UINT32_MAX)));
		fuse_fill_dir_t filler(reinterpret_cast<fuse_fill_dir_t>(::Test::rand(UINT32_MAX)));
		off_t off(::Test::rand(INT32_MAX));
		fuse_file_info fi;
		int res(::Test::rand(UINT32_MAX));
		EXPECT_CALL(*mock, readdir(buf, filler, off, Ref(fi))).WillOnce(Return(res));
		EntryProxy proxy;
		proxy = std::move(mock);
		EXPECT_EQ(proxy.readdir(buf, filler, off, fi), res);
	}
	TEST(EntryProxyTest, releasedir)
	{
		std::unique_ptr<EntryMock> mock(new EntryMock());
		fuse_file_info fi;
		int res(::Test::rand(UINT32_MAX));
		EXPECT_CALL(*mock, releasedir(Ref(fi))).WillOnce(Return(res));
		EntryProxy proxy;
		proxy = std::move(mock);
		EXPECT_EQ(proxy.releasedir(fi), res);
	}
	TEST(EntryProxyTest, mknod)
	{
		std::unique_ptr<EntryMock> mock(new EntryMock());
		const char* name(reinterpret_cast<const char*>(::Test::rand(UINT32_MAX)));
		mode_t mode(::Test::rand(UINT32_MAX));
		dev_t type(::Test::rand(UINT32_MAX));
		int res(::Test::rand(UINT32_MAX));
		EXPECT_CALL(*mock, mknod(name, mode, type)).WillOnce(Return(res));
		EntryProxy proxy;
		proxy = std::move(mock);
		EXPECT_EQ(proxy.mknod(name, mode, type), res);
	}
	TEST(EntryProxyTest, unlink)
	{
		std::unique_ptr<EntryMock> mock(new EntryMock());
		const char* name(reinterpret_cast<const char*>(::Test::rand(UINT32_MAX)));
		int res(::Test::rand(UINT32_MAX));
		EXPECT_CALL(*mock, unlink(name)).WillOnce(Return(res));
		EntryProxy proxy;
		proxy = std::move(mock);
		EXPECT_EQ(proxy.unlink(name), res);
	}
	TEST(EntryProxyTest, mkdir)
	{
		std::unique_ptr<EntryMock> mock(new EntryMock());
		const char* name(reinterpret_cast<const char*>(::Test::rand(UINT32_MAX)));
		mode_t mode(::Test::rand(UINT32_MAX));
		int res(::Test::rand(UINT32_MAX));
		EXPECT_CALL(*mock, mkdir(name, mode)).WillOnce(Return(res));
		EntryProxy proxy;
		proxy = std::move(mock);
		EXPECT_EQ(proxy.mkdir(name, mode), res);
	}
	TEST(EntryProxyTest, rmdir)
	{
		std::unique_ptr<EntryMock> mock(new EntryMock());
		const char* name(reinterpret_cast<const char*>(::Test::rand(UINT32_MAX)));
		int res(::Test::rand(UINT32_MAX));
		EXPECT_CALL(*mock, rmdir(name)).WillOnce(Return(res));
		EntryProxy proxy;
		proxy = std::move(mock);
		EXPECT_EQ(proxy.rmdir(name), res);
	}
	TEST(EntryProxyTest, flush)
	{
		std::unique_ptr<EntryMock> mock(new EntryMock());
		fuse_file_info fi;
		int res(::Test::rand(UINT32_MAX));
		EXPECT_CALL(*mock, flush(Ref(fi))).WillOnce(Return(res));
		EntryProxy proxy;
		proxy = std::move(mock);
		EXPECT_EQ(proxy.flush(fi), res);
	}
	TEST(EntryProxyTest, truncate)
	{
		std::unique_ptr<EntryMock> mock(new EntryMock());
		off_t offset(::Test::rand(UINT32_MAX));
		int res(::Test::rand(UINT32_MAX));
		EXPECT_CALL(*mock, truncate(offset)).WillOnce(Return(res));
		EntryProxy proxy;
		proxy = std::move(mock);
		EXPECT_EQ(proxy.truncate(offset), res);
	}
	TEST(EntryProxyTest, utimens)
	{
		std::unique_ptr<EntryMock> mock(new EntryMock());
		timespec times[2]{{::Test::rand(UINT32_MAX), ::Test::rand(UINT32_MAX)},
			{::Test::rand(UINT32_MAX), ::Test::rand(UINT32_MAX)}};
		int res(::Test::rand(UINT32_MAX));
		EXPECT_CALL(*mock, utimens(times)).WillOnce(Return(res));
		EntryProxy proxy;
		proxy = std::move(mock);
		EXPECT_EQ(proxy.utimens(times), res);
	}
	TEST(EntryProxyTest, readlink)
	{
		std::unique_ptr<EntryMock> mock(new EntryMock());
		char* buf(reinterpret_cast<char*>(::Test::rand(UINT32_MAX)));
		size_t len(::Test::rand(UINT32_MAX));
		int res(::Test::rand(UINT32_MAX));
		EXPECT_CALL(*mock, readlink(buf, len)).WillOnce(Return(res));
		EntryProxy proxy;
		proxy = std::move(mock);
		EXPECT_EQ(proxy.readlink(buf, len), res);
	}
	TEST(EntryProxyTest, symlink)
	{
		std::unique_ptr<EntryMock> mock(new EntryMock());
		const char* name(reinterpret_cast<char*>(::Test::rand(UINT32_MAX)));
		const char* target(reinterpret_cast<char*>(::Test::rand(UINT32_MAX)));
		int res(::Test::rand(UINT32_MAX));
		EXPECT_CALL(*mock, symlink(name, target)).WillOnce(Return(res));
		EntryProxy proxy;
		proxy = std::move(mock);
		EXPECT_EQ(proxy.symlink(name, target), res);
	}
	TEST(EntryProxyTest, setxattr)
	{
		std::unique_ptr<EntryMock> mock(new EntryMock());
		const char* name(reinterpret_cast<char*>(::Test::rand(UINT32_MAX)));
		const char* value(reinterpret_cast<char*>(::Test::rand(UINT32_MAX)));
		size_t len(::Test::rand(UINT32_MAX));
		int flags(::Test::rand(INT32_MAX));
		int res(::Test::rand(UINT32_MAX));
		EXPECT_CALL(*mock, setxattr(name, value, len, flags)).WillOnce(Return(res));
		EntryProxy proxy;
		proxy = std::move(mock);
		EXPECT_EQ(proxy.setxattr(name, value, len, flags), res);
	}
	TEST(EntryProxyTest, getxattr)
	{
		std::unique_ptr<EntryMock> mock(new EntryMock());
		const char* name(reinterpret_cast<char*>(::Test::rand(UINT32_MAX)));
		char* value(reinterpret_cast<char*>(::Test::rand(UINT32_MAX)));
		size_t len(::Test::rand(UINT32_MAX));
		int res(::Test::rand(UINT32_MAX));
		EXPECT_CALL(*mock, getxattr(name, value, len)).WillOnce(Return(res));
		EntryProxy proxy;
		proxy = std::move(mock);
		EXPECT_EQ(proxy.getxattr(name, value, len), res);
	}
	TEST(EntryProxyTest, listxattr)
	{
		std::unique_ptr<EntryMock> mock(new EntryMock());
		char* list(reinterpret_cast<char*>(::Test::rand(UINT32_MAX)));
		size_t len(::Test::rand(UINT32_MAX));
		int res(::Test::rand(UINT32_MAX));
		EXPECT_CALL(*mock, listxattr(list, len)).WillOnce(Return(res));
		EntryProxy proxy;
		proxy = std::move(mock);
		EXPECT_EQ(proxy.listxattr(list, len), res);
	}
	TEST(EntryProxyTest, removexattr)
	{
		std::unique_ptr<EntryMock> mock(new EntryMock());
		const char* name(reinterpret_cast<char*>(::Test::rand(UINT32_MAX)));
		int res(::Test::rand(UINT32_MAX));
		EXPECT_CALL(*mock, removexattr(name)).WillOnce(Return(res));
		EntryProxy proxy;
		proxy = std::move(mock);
		EXPECT_EQ(proxy.removexattr(name), res);
	}
} // namespace Test::ZipDirFs::Fuse
