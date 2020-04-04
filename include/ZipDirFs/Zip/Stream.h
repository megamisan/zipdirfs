/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_ZIP_STREAM_H
#define ZIPDIRFS_ZIP_STREAM_H

#include <istream>

#ifdef BUILD_TEST
namespace Test::ZipDirFs::Zip::Fixtures
{
	class StreamBufferInitializer;
}
#endif

namespace boost::filesystem
{
	struct path;
}

namespace std
{
	template <typename _Tp>
	struct shared_ptr;
}

namespace ZipDirFs::Zip
{
	struct Archive;
	struct Entry;
	namespace Base { struct Content; }
	class Stream : public std::istream
	{
	public:
		Stream(const boost::filesystem::path&, const std::string&);
		Stream(Archive&, const std::string&);
		Stream(Entry&);
		~Stream();
	private:
		static std::basic_streambuf<char_type>* createBuffer(const std::shared_ptr<Base::Content>&);
#ifdef BUILD_TEST
		friend class ::Test::ZipDirFs::Zip::Fixtures::StreamBufferInitializer;
#endif
	};

} // namespace ZipDirFs::Zip

#endif // ZIPDIRFS_ZIP_STREAM_H
