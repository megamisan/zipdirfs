/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_ZIP_ARCHIVE_H
#define ZIPDIRFS_ZIP_ARCHIVE_H

#include "ZipDirFs/Zip/Base/Lib.h"
#include "ZipDirFs/Zip/Entry.h"
#include <iterator>
#include <map>
#include <memory>
#include <mutex>
#include <tuple>
#include <vector>

namespace boost::filesystem
{
	struct path;
}

namespace ZipDirFs::Zip
{
	/**
	 * @brief Represents a zip archive
	 * @remarks Must be created using @link ZipDirFs::Zip::Factory::open Factory::open @endlink.
	 * @authors Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	class Archive
	{
	public:
		class ArchiveIterator;
		typedef ArchiveIterator iterator;
		static std::shared_ptr<Archive> create(Base::Lib*);
		~Archive();
		iterator begin();
		iterator begin(const std::string&);
		iterator end();
		std::shared_ptr<Entry> open(const std::string&);

		struct ArchiveIterator
			: public std::iterator<std::forward_iterator_tag, const std::string, size_t>
		{
			ArchiveIterator(ArchiveIterator&&) = default;
			ArchiveIterator(const ArchiveIterator&) = default;
			ArchiveIterator& operator=(ArchiveIterator&&) = default;
			ArchiveIterator& operator=(const ArchiveIterator&) = default;
			~ArchiveIterator();
			reference operator*() const;
			ArchiveIterator& operator++();
			bool operator==(const ArchiveIterator&) const;
			void swap(ArchiveIterator&);

		protected:
			ArchiveIterator(const std::shared_ptr<Archive>&);
			ArchiveIterator(const std::shared_ptr<Archive>&, std::vector<std::string>::iterator&&);
			ArchiveIterator(const std::shared_ptr<Archive>&, const std::string&);
			std::shared_ptr<Archive> archive;
			std::shared_ptr<std::string> prefix;
			std::vector<std::string>::iterator current;
			std::string filename;
			friend class Archive;
		};

	protected:
		Archive(Base::Lib*);
		void populate();
		std::weak_ptr<Archive> weak_ptr;
		std::mutex names_mutex;
		std::mutex entries_mutex;
		Base::Lib* const data;
		std::vector<std::string> names;
		std::map<std::string, std::tuple<std::uint64_t, bool>> nameAttributes;
		std::map<std::uint64_t, std::weak_ptr<Entry>> entries;
		friend class ArchiveIterator;
	};

} // namespace ZipDirFs::Zip

namespace std
{
	using ZipDirFs::Zip::Archive;
	void swap(Archive::ArchiveIterator& it1, Archive::ArchiveIterator& it2);

} // namespace std

#endif // ZIPDIRFS_ZIP_ARCHIVE_H
