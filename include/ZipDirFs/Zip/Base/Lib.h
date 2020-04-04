/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_ZIP_BASE_LIB_H
#define ZIPDIRFS_ZIP_BASE_LIB_H

namespace ZipDirFs::Zip::Base
{
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
