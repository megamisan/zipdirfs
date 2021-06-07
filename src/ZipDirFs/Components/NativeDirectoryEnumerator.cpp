/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/Components/NativeDirectoryEnumerator.h"
#include "ZipDirFs/Utilities/FileSystem.h"

namespace ZipDirFs::Components
{
	using ZipDirFs::Utilities::FileSystem;
	NativeDirectoryEnumerator::NativeDirectoryEnumerator(const boost::filesystem::path& p) :
		path(p), currentIt(FileSystem::directory_iterator_end()), endIt(currentIt)
	{
	}

	NativeDirectoryEnumerator::~NativeDirectoryEnumerator() {}
	void NativeDirectoryEnumerator::reset()
	{
		currentIt = FileSystem::directory_iterator_from_path(path);
	}
	void NativeDirectoryEnumerator::next() { ++currentIt; }
	bool NativeDirectoryEnumerator::valid() { return !(currentIt == endIt); }
	const std::string& NativeDirectoryEnumerator::current() { return *currentIt; }

} // namespace ZipDirFs::Components
