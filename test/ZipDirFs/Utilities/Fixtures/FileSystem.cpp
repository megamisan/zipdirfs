/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "FileSystem.h"
#include <boost/filesystem.hpp>

namespace Test::ZipDirFs::Utilities
{
	namespace Fixtures
	{
		using BaseFileSystem = ::ZipDirFs::Utilities::FileSystem;

		FileSystem::FileSystem()
		{
			BaseFileSystem::last_write_time = [this](const path& path) {
				return this->last_write_time(path);
			};
			BaseFileSystem::directory_iterator_from_path = [this](const path& path) {
				return EntryIterator(this->directory_iterator_from_path(path));
			};
			BaseFileSystem::directory_iterator_end = [this] {
				return EntryIterator(this->directory_iterator_end());
			};
			BaseFileSystem::status = [this](const path& path) {
				return this->status(path);
			};
		}

		FileSystem::~FileSystem() { BaseFileSystem::reset(); }

	} // namespace Fixtures
} // namespace Test::ZipDirFs::Utilities
