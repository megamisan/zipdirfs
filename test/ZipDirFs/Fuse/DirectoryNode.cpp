/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "DirectoryNode.h"
#include "Fixtures/EmptyEnumerator.h"
#include "Fixtures/FunctionalEnumerator.h"
#include "Fixtures/FuseDaemonFork.h"
#include "Fixtures/NullFactory.h"
#include "Fixtures/OneChanged.h"
#include "ZipDirFs/Containers/EntryList.h"
#include "ZipDirFs/Fuse/DirectoryNode.h"
#include "ZipDirFs/Fuse/EntryProxy.h"
#include "ZipDirFs/Fuse/NativeTimePolicy.h"
#include "test/gtest.h"
#include <boost/filesystem.hpp>
#include <fusekit/basic_directory.h>
#include <fusekit/default_permissions.h>
#include <fusekit/no_xattr.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <poll.h>

namespace Test::ZipDirFs::Fuse
{
	using namespace ::boost;

	namespace
	{
		using ::ZipDirFs::Containers::EntryList;
		using namespace ::ZipDirFs::Fuse;
		using namespace Fixtures;
		using namespace fusekit;

		filesystem::path tempFolderPath()
		{
			return filesystem::temp_directory_path()
				/ filesystem::unique_path("test-%%%%-%%%%-%%%%-%%%%");
		}

		struct EmptyRootDirectory : public basic_directory<DefaultDirectoryNode, NativeTimePolicy,
										default_directory_permissions, no_xattr>
		{
			EmptyRootDirectory(std::time_t now, std::time_t modified);
			std::time_t getChangeTime() const;
			std::time_t getModificationTime() const;
			EntryGenerator& generator();
			EntryGenerator::proxy_ptr& proxy();
			EntryGenerator::locker_ptr& locker();

		protected:
			const std::time_t changeTime;
			EntryGenerator::proxy_ptr _proxy;
			EntryGenerator::changed_ptr _changed;
			EntryGenerator::locker_ptr _locker;
			EntryGenerator _generator;
		};

		struct NotifiedEntryGenerator : public EntryGenerator
		{
			NotifiedEntryGenerator(proxy_ptr&& p, changed_ptr&& c, enumerator_ptr&& e,
				factory_ptr&& f, locker_ptr&& l) :
				EntryGenerator(std::move(p), std::move(c), std::move(e), std::move(f), std::move(l))
			{
			}
			void add(::ZipDirFs::Containers::EntryIterator&& it, off_t off)
			{
				hadAdd = true;
				EntryGenerator::add(std::move(it), off);
			}
			bool hadAdd = false;
		};

		template <class Derived>
		struct NotifiedDirectoryNode : public DirectoryNode<Derived, NotifiedEntryGenerator>
		{
		};

		struct FunctionalRootDirectory
			: public basic_directory<NotifiedDirectoryNode, NativeTimePolicy,
				  default_directory_permissions, no_xattr>
		{
			FunctionalRootDirectory(
				NotifiedEntryGenerator::enumerator_ptr& enumerator, std::time_t modified);
			std::time_t getChangeTime() const;
			std::time_t getModificationTime() const;
			NotifiedEntryGenerator& generator();
			NotifiedEntryGenerator::proxy_ptr& proxy();
			EntryGenerator::locker_ptr& locker();

		protected:
			NotifiedEntryGenerator::proxy_ptr _proxy;
			NotifiedEntryGenerator::changed_ptr _changed;
			NotifiedEntryGenerator::locker_ptr _locker;
			NotifiedEntryGenerator _generator;
		};

		EmptyRootDirectory::EmptyRootDirectory(std::time_t now, std::time_t modified) :
			_proxy(EntryList<>::createWithProxy()), _changed(new OneChanged(modified)),
			_locker(new EntryGenerator::locker_type()),
			_generator(EntryGenerator::proxy_ptr(_proxy), EntryGenerator::changed_ptr(_changed),
				EntryGenerator::enumerator_ptr(new EmptyEnumerator()),
				EntryGenerator::factory_ptr(new NullFactory()),
				EntryGenerator::locker_ptr(_locker)),
			changeTime(now)
		{
		}
		std::time_t EmptyRootDirectory::getChangeTime() const { return changeTime; }
		std::time_t EmptyRootDirectory::getModificationTime() const { return *_changed; }
		EntryGenerator& EmptyRootDirectory::generator() { return _generator; }
		EntryGenerator::proxy_ptr& EmptyRootDirectory::proxy() { return _proxy; }
		EntryGenerator::locker_ptr& EmptyRootDirectory::locker() { return _locker; }

		FunctionalRootDirectory::FunctionalRootDirectory(
			NotifiedEntryGenerator::enumerator_ptr& enumerator, std::time_t modified) :
			_proxy(EntryList<>::createWithProxy()),
			_changed(new OneChanged(modified)), _locker(new NotifiedEntryGenerator::locker_type()),
			_generator(NotifiedEntryGenerator::proxy_ptr(_proxy),
				NotifiedEntryGenerator::changed_ptr(_changed),
				NotifiedEntryGenerator::enumerator_ptr(enumerator),
				NotifiedEntryGenerator::factory_ptr(new NullFactory()),
				EntryGenerator::locker_ptr(_locker))
		{
		}
		std::time_t FunctionalRootDirectory::getChangeTime() const { return *_changed; }
		std::time_t FunctionalRootDirectory::getModificationTime() const { return *_changed; }
		NotifiedEntryGenerator& FunctionalRootDirectory::generator() { return _generator; }
		NotifiedEntryGenerator::proxy_ptr& FunctionalRootDirectory::proxy() { return _proxy; }
		NotifiedEntryGenerator::locker_ptr& FunctionalRootDirectory::locker() { return _locker; }

		struct Guard
		{
			Guard(std::function<void()> f) : _f(f){};
			~Guard() { _f(); }
			void finalize()
			{
				_f();
				_f = []() {};
			}

		protected:
			std::function<void()> _f;
		};

		struct DirectoryNodeListGenerator
		{
			DirectoryNodeListGenerator(NotifiedEntryGenerator*& generator) : _generator(generator)
			{
			}
			void reset()
			{
				source.clear();
				_start = true;
				_max = 1000;
				_current = 2;
				source.push_back(std::string("."));
				source.push_back(std::string(".."));
				source.push_back("item" + std::to_string(_current));
				expected.push_back("item" + std::to_string(_current));
			}
			void next()
			{
				if (_start && _generator->hadAdd)
				{
					_start = false;
					_max = source.size() + 10;
				}
				++_current;
				if (_current < _max && _current > 1)
				{
					source.push_back("item" + std::to_string(_current));
					expected.push_back("item" + std::to_string(_current));
				}
			}
			bool valid() { return _current < _max; }
			const std::string& current() { return source[_current]; }
			std::vector<std::string> source;
			std::vector<std::string> expected;

		private:
			NotifiedEntryGenerator*& _generator;
			int _max = 1000;
			int _current = 0;
			bool _start = true;
		};
	} // namespace

#define SKIP_IF_NO_FUSE \
	if (!::boost::filesystem::exists("/dev/fuse")) { \
		GTEST_SKIP_("Fuse not found."); \
		return; \
	}

	TEST(DirectoryNodeTest, Stat)
	{
		SKIP_IF_NO_FUSE;
		std::time_t now(time(NULL)), modified((std::time_t)::Test::rand(now));
		filesystem::path mountPoint(tempFolderPath());
		filesystem::create_directory(mountPoint);
		std::string fsName = "DirectoryNodeTestStat";
		Guard rmdir(
			[mountPoint]()
			{
				try
				{
					filesystem::remove(mountPoint);
				}
				catch (boost::filesystem::filesystem_error e)
				{
				}
			});
		int statResult(-1);
		struct stat stbuf
		{
			0
		};
		FuseDaemonFork daemon(
			mountPoint.native(), fsName,
			std::unique_ptr<::fusekit::entry>(new EmptyRootDirectory(now, modified)),
			[&daemon, &mountPoint, &statResult, &stbuf](std::vector<int> fds) -> void
			{
				Guard readFd([&fds]() { close(fds[0]); });
				Guard unmount(std::bind(std::mem_fn(&FuseDaemonFork::stop), &daemon));
				struct pollfd descriptors[1] = {{fds[0], POLLIN, 0}};
				ppoll(descriptors, 1, nullptr, nullptr);
				if (read(fds[0], &statResult, sizeof(int)) < 0)
				{
					perror("Worker");
				}
				if (read(fds[0], &stbuf, sizeof(struct stat)) < 0)
				{
					perror("Worker");
				}
			},
			"FileStat", {0, 1}, std::vector<std::string>({mountPoint.native()}));
		EXPECT_EQ(statResult, 0);
		ASSERT_EQ(stbuf.st_nlink, 2);
		ASSERT_EQ(stbuf.st_ctim.tv_sec, now);
		ASSERT_EQ(stbuf.st_mtim.tv_sec, 0);
	}

	TEST(DirectoryNodeTest, List)
	{
		SKIP_IF_NO_FUSE;
		std::time_t now(time(NULL)), modified((std::time_t)::Test::rand(now));
		filesystem::path mountPoint(tempFolderPath());
		filesystem::create_directory(mountPoint);
		std::string fsName = "DirectoryNodeTestList";
		Guard rmdir(
			[mountPoint]()
			{
				try
				{
					filesystem::remove(mountPoint);
				}
				catch (boost::filesystem::filesystem_error e)
				{
				}
			});
		NotifiedEntryGenerator* generator;
		DirectoryNodeListGenerator listGenerator(generator);
		NotifiedEntryGenerator::enumerator_ptr enumerator(new FunctionalEnumerator(
			std::bind(std::mem_fn(&DirectoryNodeListGenerator::reset), &listGenerator),
			std::bind(std::mem_fn(&DirectoryNodeListGenerator::next), &listGenerator),
			std::bind(std::mem_fn(&DirectoryNodeListGenerator::valid), &listGenerator),
			std::bind(std::mem_fn(&DirectoryNodeListGenerator::current), &listGenerator)));
		FunctionalRootDirectory* root = new FunctionalRootDirectory(enumerator, modified);
		generator = &root->generator();
		std::vector<std::string> result;
		FuseDaemonFork daemon(
			mountPoint.native(), fsName, std::unique_ptr<::fusekit::entry>(root),
			[&mountPoint, &result, &daemon](std::vector<int> fds)
			{
				Guard readFd([&fds]() { close(fds[0]); });
				Guard unmount(std::bind(std::mem_fn(&FuseDaemonFork::stop), &daemon));
				struct pollfd descriptors[1] = {{fds[0], POLLIN, 0}};
				ppoll(descriptors, 1, nullptr, nullptr);
				char buf[256];
				ssize_t res;
				std::string temp;
				while ((res = read(fds[0], buf, sizeof(buf))) > 0)
				{
					ssize_t start = 0;
					ssize_t offset = 0;
					while (offset < res)
					{
						if (buf[offset] == 0)
						{
							temp += std::string(buf + start, offset - start);
							result.push_back(temp);
							temp.clear();
							start = offset + 1;
						}
						++offset;
					}
					if (start < offset)
					{
						temp += std::string(buf + start, offset - start);
					}
				}
				if (res < 0)
				{
					perror("Worker");
				}
			},
			"DirectoryNodeList", {0, 1}, std::vector<std::string>({mountPoint.native()}));
		EXPECT_GT(result.size(), 0);
		ASSERT_EQ(result, listGenerator.expected);
	}
} // namespace Test::ZipDirFs::Fuse
