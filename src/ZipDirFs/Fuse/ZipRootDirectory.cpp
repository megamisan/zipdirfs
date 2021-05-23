/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/Fuse/ZipRootDirectory.h"
#include "ZipDirFs/Components/ChangedStart.h"
#include "ZipDirFs/Components/NativeChanged.h"
#include "ZipDirFs/Components/ZipDirectoryEnumerator.h"
#include "ZipDirFs/Components/ZipFactory.h"
#include "ZipDirFs/Containers/EntryGenerator.h"
#include "ZipDirFs/Containers/EntryList.h"
#include "ZipDirFs/Fuse/ZipDirectory.h"
#include "ZipDirFs/Fuse/ZipFile.h"
#include <boost/filesystem.hpp>

namespace ZipDirFs::Fuse
{
	namespace
	{
		using namespace ::ZipDirFs::Components;
		using ::ZipDirFs::Containers::Helpers::Changed;

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

		inline std::shared_ptr<::ZipDirFs::Components::ChangedProxy> getChangedProxy(
			const boost::filesystem::path& p,
			std::shared_ptr<::ZipDirFs::Components::ChangedProxy>& t)
		{
			return std::shared_ptr<ChangedProxy>(new ChangedProxy(std::unique_ptr<Changed>(
				std::unique_ptr<ChangedStart>(new ChangedStart(FirstValue(p), BuildReal(p), t)))));
		}
	} // namespace

	std::string ZipRootDirectoryItem("");

	ZipRootDirectory::ZipRootDirectory(const boost::filesystem::path& p) :
		path(p), _proxy(std::move(::ZipDirFs::Containers::EntryList<>::createWithProxy())),
		_changed(getChangedProxy(path, _changed)),
		_generator(EntryGenerator::proxy_ptr(_proxy), EntryGenerator::changed_ptr(_changed),
			EntryGenerator::enumerator_ptr(
				new ::ZipDirFs::Components::ZipDirectoryEnumerator(path, ZipRootDirectoryItem)),
			EntryGenerator::factory_ptr(
				new ::ZipDirFs::Components::ZipFactory<ZipDirectory, ZipFile>(
					path, ZipRootDirectoryItem, EntryGenerator::changed_ptr(_changed))))
	{
	}
	std::time_t ZipRootDirectory::getChangeTime() const { return *_changed; }
	std::time_t ZipRootDirectory::getModificationTime() const { return *_changed; }
	EntryGenerator& ZipRootDirectory::generator() { return _generator; }
	EntryGenerator::proxy_ptr& ZipRootDirectory::proxy() { return _proxy; }
} // namespace ZipDirFs::Fuse
