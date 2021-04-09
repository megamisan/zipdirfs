/*
 * Copyright © 2012-2019 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/ZipWalker.h"
#include "ZipDirFs/ZipFile.h"
#include "ZipDirFs/entry_definitions.h"
#include <cstring>

namespace ZipDirFs
{
	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	void ZipWalker::advance()
	{
		this->advanceInit();
		this->advanceToNext();
	}

	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	void ZipWalker::advanceInit()
	{
		if (begin != end)
		{
			this->entry.first = "";
			this->entry.second = NULL;
			begin++;
		}
	}

	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	void ZipWalker::advanceToNext()
	{
		while (begin != end)
		{
			if ((this->filterPath.size() == 0) || (::strncmp(begin->name.c_str(), this->filterPath.c_str(), this->filterPath.size()) == 0))
			{
				std::string name(begin->name.substr(this->filterPath.size()));

				if (name.size() > 0)
				{
					::size_t pos = name.find('/');

					if ((pos == std::string::npos) || (pos + 1 == name.size()))
					{
						this->entry.first = name;

						if (pos != std::string::npos)
						{
							this->entry.first.erase(pos);
							zip_directory* dir = new zip_directory();
							dir->setDirectoryInfo(zipFile, begin->name, begin->mtime);
							this->entry.second = dir;
						}
						else
						{
							zip_file* file = new zip_file();
							file->setFileInfo(this->zipFile->getEntry(*begin));
							this->entry.second = file;
						}
					}
				}
			}

			if (this->entry.second != NULL)
			{
				break;
			}

			begin++;
		}
	}

	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	ZipWalker::~ZipWalker()
	{
	}

	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	ZipWalker::pointer ZipWalker::operator->() const
	{
		return &this->entry;
	}

	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	ZipWalker::reference ZipWalker::operator*() const
	{
		return this->entry;
	}

	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	bool ZipWalker::operator!= (const ZipWalker& it) const
	{
		return (this->begin != it.begin) || (this->end != it.end) || (this->filterPath != it.filterPath);
	}

	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	bool ZipWalker::operator== (const ZipWalker& it) const
	{
		return (this->begin == it.begin) || (this->end == it.end) || (this->filterPath == it.filterPath);
	}

	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	ZipWalker& ZipWalker::operator++()
	{
		this->advance();
		return *this;
	}

	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	ZipWalker& ZipWalker::operator= (const ZipWalker& it)
	{
		this->zipFile = it.zipFile;
		this->begin = it.begin;
		this->end = it.end;
		this->filterPath = it.filterPath;
		this->entry = it.entry;
		return *this;
	}

	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	ZipWalker::ZipWalker(const ZipWalker& it) : zipFile(it.zipFile), begin(it.begin), end(it.end), filterPath(it.filterPath), entry(it.entry)
	{
	}

	/** @brief (one liner)
	  *
	  * (documentation goes here)
	  */
	ZipWalker::ZipWalker(ZipFile* zipFile, std::string filterPath, bool end) : zipFile(zipFile), begin(end ? zipFile->end() : zipFile->begin()), end(zipFile->end()), filterPath(filterPath)
	{
		this->entry.second = NULL;

		if (!end)
		{
			this->advanceToNext();
		}
	}
} // namespace ZipDirFs
