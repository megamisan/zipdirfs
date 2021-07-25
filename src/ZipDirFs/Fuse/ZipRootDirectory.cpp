/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/Fuse/ZipRootDirectory.h"
#include "ZipDirFs/Components/ChangedStart.h"
#include "ZipDirFs/Components/EntryListProxyIncremental.h"
#include "ZipDirFs/Components/NativeChanged.h"
#include "ZipDirFs/Components/ZipDirectoryEnumerator.h"
#include "ZipDirFs/Components/ZipFactory.h"
#include "ZipDirFs/Containers/EntryGenerator.h"
#include "ZipDirFs/Containers/EntryList.h"
#include "ZipDirFs/Fuse/ZipDirectory.h"
#include "ZipDirFs/Fuse/ZipFile.h"
#include "ZipDirFs/Utilities/list_cleanup.h"
#include <boost/filesystem.hpp>

namespace ZipDirFs::Fuse
{
	namespace
	{
		using namespace ::ZipDirFs::Components;
		using ::ZipDirFs::Containers::Helpers::Changed;
		using ::ZipDirFs::Containers::Helpers::EntryListProxy;

		struct FirstValue
		{
			FirstValue(const boost::filesystem::path& path) : path(path) {}
			const std::time_t operator()() const { return NativeChanged::getTime(path); }

		private:
			const boost::filesystem::path& path;
		};

		struct BuildReal
		{
			BuildReal(const boost::filesystem::path& path) : path(path) {}
			std::unique_ptr<Changed> operator()() const
			{
				return std::unique_ptr<Changed>(
					std::unique_ptr<NativeChanged>(new NativeChanged(path)));
			}

		private:
			const boost::filesystem::path& path;
		};

		struct Resolve
		{
			Resolve(const EntryGenerator::factory_ptr& factory) : factory(factory) {}
			fusekit::entry* operator()(const std::string& name) const
			{
				return factory->create(name);
			}

		private:
			EntryGenerator::factory_ptr factory;
		};

		inline std::shared_ptr<ChangedProxy> getChangedProxy(
			const boost::filesystem::path& p, std::shared_ptr<ChangedProxy>& t)
		{
			return std::shared_ptr<ChangedProxy>(new ChangedProxy(std::unique_ptr<Changed>(
				std::unique_ptr<ChangedStart>(new ChangedStart(FirstValue(p), BuildReal(p), t)))));
		}
		inline std::shared_ptr<EntryListProxy> getEntryListProxy(
			const EntryGenerator::factory_ptr& factory,
			const std::shared_ptr<EntryListProxy>& proxy)
		{
			return std::shared_ptr<EntryListProxy>(
				new EntryListProxyProxy(std::unique_ptr<EntryListProxy>(
					std::unique_ptr<EntryListProxyIncremental>(new EntryListProxyIncremental(
						Resolve(factory), EntryGenerator::proxy_ptr(proxy))))));
		}
	} // namespace

	std::string ZipRootDirectoryItem("");

	ZipRootDirectory::ZipRootDirectory(const boost::filesystem::path& p) :
		path(p), _changed(getChangedProxy(path, _changed)),
		_factory(new ::ZipDirFs::Components::ZipFactory<ZipDirectory, ZipFile>(
			path, ZipRootDirectoryItem, EntryGenerator::changed_ptr(_changed))),
		_proxyBase(std::move(::ZipDirFs::Containers::EntryList<>::createWithProxy())),
		_proxy(getEntryListProxy(_factory, _proxyBase)), _locker(new EntryGenerator::locker_type()),
		_generator(EntryGenerator::proxy_ptr(_proxyBase), EntryGenerator::changed_ptr(_changed),
			EntryGenerator::enumerator_ptr(
				new ::ZipDirFs::Components::ZipDirectoryEnumerator(path, ZipRootDirectoryItem)),
			EntryGenerator::factory_ptr(_factory), EntryGenerator::locker_ptr(_locker))
	{
	}
	ZipRootDirectory::~ZipRootDirectory()
	{
		::ZipDirFs::Utilities::CleanupEntryList(*_proxyBase, *_factory);
	}
	std::time_t ZipRootDirectory::getChangeTime() const { return *_changed; }
	std::time_t ZipRootDirectory::getModificationTime() const { return *_changed; }
	EntryGenerator& ZipRootDirectory::generator() { return _generator; }
	EntryGenerator::proxy_ptr& ZipRootDirectory::proxy() { return _proxy; }
	EntryGenerator::locker_ptr& ZipRootDirectory::locker() { return _locker; }

	int ZipRootDirectory::opendir(fuse_file_info& finfo)
	{
		try
		{
			generator().begin();
		}
		catch (ZipDirFs::Zip::Exception ex)
		{
			if (ex.code() != 0)
			{
				return -ex.code();
			}
			return -EIO;
		}
		return base<DefaultDirectoryNode>().opendir(finfo);
	}

} // namespace ZipDirFs::Fuse
