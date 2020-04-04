/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_FUSEKIT_FIXTURES_ENTRYMOCK_H
#define TEST_FUSEKIT_FIXTURES_ENTRYMOCK_H

#include <fusekit/entry.h>
#include <gmock/gmock.h>

namespace Test::fusekit
{
	namespace Fixtures
	{
		struct EntryMock : public ::fusekit::entry
		{
			MOCK_METHOD1(child, ::fusekit::entry*(const char*));
			MOCK_METHOD1(stat, int(struct stat&));
			MOCK_METHOD1(access, int(int));
			MOCK_METHOD1(chmod, int(mode_t));
			MOCK_METHOD1(open, int(fuse_file_info&));
			MOCK_METHOD1(release, int(fuse_file_info&));
			MOCK_METHOD4(read, int(char*, size_t, off_t, fuse_file_info&));
			MOCK_METHOD4(write, int(const char*, size_t, off_t, fuse_file_info&));
			MOCK_METHOD1(opendir, int(fuse_file_info&));
			MOCK_METHOD4(readdir, int(void*, fuse_fill_dir_t, off_t, fuse_file_info&));
			MOCK_METHOD1(releasedir, int(fuse_file_info&));
			MOCK_METHOD3(mknod, int(const char*, mode_t, dev_t));
			MOCK_METHOD1(unlink, int(const char*));
			MOCK_METHOD2(mkdir, int(const char*, mode_t));
			MOCK_METHOD1(rmdir, int(const char*));
			MOCK_METHOD1(flush, int(fuse_file_info&));
			MOCK_METHOD1(truncate, int(off_t));
			MOCK_METHOD1(utimens, int(const timespec[2]));
			MOCK_METHOD2(readlink, int(char*, size_t));
			MOCK_METHOD2(symlink, int(const char*, const char*));
			MOCK_METHOD4(setxattr, int(const char*, const char*, size_t, int));
			MOCK_METHOD3(getxattr, int(const char*, char*, size_t));
			MOCK_METHOD2(listxattr, int(char*, size_t));
			MOCK_METHOD1(removexattr, int(const char*));
		};

	} // namespace Fixtures
} // namespace Test::fusekit

#endif // TEST_FUSEKIT_FIXTURES_ENTRYMOCK_H
