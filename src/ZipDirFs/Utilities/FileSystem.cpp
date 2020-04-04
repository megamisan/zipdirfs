/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/Utilities/FileSystem.h"
#include <boost/filesystem.hpp>

namespace ZipDirFs::Utilities
{
	using namespace boost;

	namespace
	{
		struct DirectoryIteratorWrapper : public EntryIterator::Wrapper
		{
			DirectoryIteratorWrapper(directory_iterator&& it);
			DirectoryIteratorWrapper(const directory_iterator& it);
			~DirectoryIteratorWrapper();

		protected:
			Wrapper* clone() const;
			reference dereference() const;
			void increment();
			bool equals(const Wrapper&) const;
			directory_iterator currentIt;
			std::string value;
			bool valid;
		};

		EntryIterator fileSystemDirectoryIteratorFromPath(const path& p)
		{
			return EntryIterator(new DirectoryIteratorWrapper(directory_iterator(p)));
		}

		EntryIterator fileSystemDirectoryIteratorEnd()
		{
			return EntryIterator(new DirectoryIteratorWrapper(directory_iterator()));
		}

		filesystem::file_status fileSystemStatus(const path& p) {
			return filesystem::status(p);
		}
	} // namespace

	FileSystem::FileSystem() {}

	void FileSystem::reset()
	{
		assign(FileSystem::last_write_time, filesystem::last_write_time);
		assign(FileSystem::directory_iterator_from_path, fileSystemDirectoryIteratorFromPath);
		assign(FileSystem::directory_iterator_end, fileSystemDirectoryIteratorEnd);
		assign(FileSystem::status, fileSystemStatus);
	}

	std::function<std::time_t(const path&)> FileSystem::last_write_time;
	std::function<EntryIterator(const path&)> FileSystem::directory_iterator_from_path;
	std::function<EntryIterator()> FileSystem::directory_iterator_end;
	std::function<filesystem::file_status(const path&)> FileSystem::status;

	bool init = [] {
		FileSystem::reset();
		return true;
	}();

	DirectoryIteratorWrapper::DirectoryIteratorWrapper(directory_iterator&& it) :
		currentIt(std::move(it)), valid(false), value("")
	{
	}
	DirectoryIteratorWrapper::DirectoryIteratorWrapper(const directory_iterator& it) : currentIt(it), valid(false), value("")
	{
	}
	DirectoryIteratorWrapper::~DirectoryIteratorWrapper() {}
	DirectoryIteratorWrapper::Wrapper* DirectoryIteratorWrapper::clone() const
	{
		return new DirectoryIteratorWrapper(currentIt);
	}
	DirectoryIteratorWrapper::reference DirectoryIteratorWrapper::dereference() const
	{
		if (!valid) {
			*const_cast<std::string*>(&value) = currentIt->path().leaf().native();
			*const_cast<bool*>(&valid) = true;
		}
		return value;
	}
	void DirectoryIteratorWrapper::increment() { ++currentIt; valid = false; }
	bool DirectoryIteratorWrapper::equals(const Wrapper& w) const
	{
		const DirectoryIteratorWrapper* other = dynamic_cast<const DirectoryIteratorWrapper*>(&w);
		return (other != nullptr) && (currentIt == other->currentIt);
	}

} // namespace ZipDirFs::Utilities
