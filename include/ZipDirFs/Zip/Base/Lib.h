/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_ZIP_BASE_LIB_H
#define ZIPDIRFS_ZIP_BASE_LIB_H

namespace ZipDirFs::Zip::Base
{
	/**
	 * @brief Helper base objects to hold implementation defined handles
	 * @authors Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	class Lib
	{
	public:
		class File
		{
		protected:
			File();
			~File();
		};

	protected:
		Lib();
		~Lib();

	private:
	};

} // namespace ZipDirFs::Zip::Base

#endif // ZIPDIRFS_ZIP_BASE_LIB_H
