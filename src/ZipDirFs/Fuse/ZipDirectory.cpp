/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/Fuse/ZipDirectory.h"
#include "ZipDirFs/Components/ChangedStart.h"
#include "ZipDirFs/Components/EntryListProxyIncremental.h"
#include "ZipDirFs/Components/ZipDirectoryEnumerator.h"
#include "ZipDirFs/Components/ZipFactory.h"
#include "ZipDirFs/Components/ZipFileChanged.h"
#include "ZipDirFs/Containers/EntryGenerator.h"
#include "ZipDirFs/Containers/EntryList.h"
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
			FirstValue(const boost::filesystem::path& path, const std::string& item) :
				path(path), item(item)
			{
			}
			const std::time_t operator()() const { return ZipFileChanged::getTime(path, item); }

		private:
			const boost::filesystem::path& path;
			const std::string& item;
		};

		struct BuildReal
		{
			BuildReal(const boost::filesystem::path& path, const std::string& item) :
				path(path), item(item)
			{
			}
			std::unique_ptr<Changed> operator()() const
			{
				return std::unique_ptr<Changed>(
					std::unique_ptr<ZipFileChanged>(new ZipFileChanged(path, item)));
			}

		private:
			const boost::filesystem::path& path;
			const std::string& item;
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

		inline std::shared_ptr<ChangedProxy> getChangedProxy(const boost::filesystem::path& p,
			const std::string& f, std::shared_ptr<ChangedProxy>& t)
		{
			return std::shared_ptr<ChangedProxy>(
				new ChangedProxy(std::unique_ptr<Changed>(std::unique_ptr<ChangedStart>(
					new ChangedStart(FirstValue(p, f), BuildReal(p, f), t)))));
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

	ZipDirectory::ZipDirectory(
		const boost::filesystem::path& p, const std::string& f, EntryGenerator::changed_ptr& rc) :
		zip(p),
		folder(f), _rootChanged(rc), _changed(getChangedProxy(zip, folder, _changed)),
		_factory(new ::ZipDirFs::Components::ZipFactory<ZipDirectory, ZipFile>(
			zip, folder, EntryGenerator::changed_ptr(rc))),
		_proxyBase(std::move(::ZipDirFs::Containers::EntryList<>::createWithProxy())),
		_proxy(getEntryListProxy(_factory, _proxyBase)), _locker(new EntryGenerator::locker_type()),
		_generator(EntryGenerator::proxy_ptr(_proxyBase), EntryGenerator::changed_ptr(_changed),
			EntryGenerator::enumerator_ptr(
				new ::ZipDirFs::Components::ZipDirectoryEnumerator(zip, folder)),
			EntryGenerator::factory_ptr(_factory), EntryGenerator::locker_ptr(_locker))
	{
	}
	ZipDirectory::~ZipDirectory()
	{
		::ZipDirFs::Utilities::CleanupEntryList(*_proxyBase, *_factory);
	}
	std::time_t ZipDirectory::getParentModificationTime() const { return *_rootChanged; }
	std::time_t ZipDirectory::getModificationTime() const
	{
		return *_changed != 0 ? *_changed : *_rootChanged;
	}
	EntryGenerator& ZipDirectory::generator() { return _generator; }
	EntryGenerator::proxy_ptr& ZipDirectory::proxy() { return _proxy; }
	EntryGenerator::locker_ptr& ZipDirectory::locker() { return _locker; }
} // namespace ZipDirFs::Fuse
