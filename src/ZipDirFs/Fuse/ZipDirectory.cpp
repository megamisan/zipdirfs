/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/Fuse/ZipDirectory.h"
#include "ZipDirFs/Components/ChangedStart.h"
#include "ZipDirFs/Components/ZipDirectoryEnumerator.h"
#include "ZipDirFs/Components/ZipFactory.h"
#include "ZipDirFs/Components/ZipFileChanged.h"
#include "ZipDirFs/Containers/EntryGenerator.h"
#include "ZipDirFs/Containers/EntryList.h"
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

		inline std::shared_ptr<::ZipDirFs::Components::ChangedProxy> getChangedProxy(
			const boost::filesystem::path& p, const std::string& f,
			std::shared_ptr<::ZipDirFs::Components::ChangedProxy>& t)
		{
			return std::shared_ptr<ChangedProxy>(
				new ChangedProxy(std::unique_ptr<Changed>(std::unique_ptr<ChangedStart>(
					new ChangedStart(FirstValue(p, f), BuildReal(p, f), t)))));
		}
	} // namespace

	ZipDirectory::ZipDirectory(
		const boost::filesystem::path& p, const std::string& f, EntryGenerator::changed_ptr& rc) :
		zip(p),
		folder(f), _rootChanged(rc),
		_proxy(std::move(::ZipDirFs::Containers::EntryList<>::createWithProxy())),
		_changed(getChangedProxy(zip, folder, _changed)),
		_generator(EntryGenerator::proxy_ptr(_proxy), EntryGenerator::changed_ptr(_changed),
			EntryGenerator::enumerator_ptr(
				new ::ZipDirFs::Components::ZipDirectoryEnumerator(zip, folder)),
			EntryGenerator::factory_ptr(
				new ::ZipDirFs::Components::ZipFactory<ZipDirectory, ZipFile>(
					zip, folder, EntryGenerator::changed_ptr(rc))))
	{
	}
	std::time_t ZipDirectory::getParentModificationTime() const { return *_rootChanged; }
	std::time_t ZipDirectory::getModificationTime() const
	{
		return *_changed != 0 ? *_changed : *_rootChanged;
	}
	EntryGenerator& ZipDirectory::generator() { return _generator; }
	EntryGenerator::proxy_ptr& ZipDirectory::proxy() { return _proxy; }
} // namespace ZipDirFs::Fuse
