/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef TEST_ZIPDIRFS_ZIP_FIXTURES_STREAMBUFFERINITIALIZER_H
#define TEST_ZIPDIRFS_ZIP_FIXTURES_STREAMBUFFERINITIALIZER_H
#ifdef ZIPDIRFS_ZIP_STREAM_H
#	ifndef BUILD_TEST
#		error BUILD_TEST must be defined before including ZipDirFs/Zip/Stream.h
#	endif
#endif
#ifndef BUILD_TEST
#	define BUILD_TEST
#endif
#include "ZipDirFs/Zip/Stream.h"
#include <memory>

namespace Test::ZipDirFs::Zip
{
	namespace Fixtures
	{
		struct StreamBufferInitializer
			: public std::basic_streambuf<::ZipDirFs::Zip::Stream::char_type>
		{
			typedef ::ZipDirFs::Zip::Stream::char_type char_type;
			StreamBufferInitializer() = delete;
			static std::basic_streambuf<char_type>* create(
				const std::shared_ptr<::ZipDirFs::Zip::Base::Content>&);
			static std::shared_ptr<::ZipDirFs::Zip::Base::Content> getContent(
				std::basic_streambuf<char_type>*);
			static void invokeSetg(
				std::basic_streambuf<char_type>*, char_type*, char_type*, char_type*);
			static char_type* invokeEback(const std::basic_streambuf<char_type>*);
			static char_type* invokeGptr(const std::basic_streambuf<char_type>*);
			static char_type* invokeEgptr(const std::basic_streambuf<char_type>*);
			static std::streamsize invokeShowmanyc(std::basic_streambuf<char_type>*);
			static int_type invokeUnderflow(std::basic_streambuf<char_type>*);
			static std::streamsize invokeXsgetn(
				std::basic_streambuf<char_type>*, char_type*, std::streamsize);

		private:
			const std::shared_ptr<::ZipDirFs::Zip::Base::Content> content;
		};

	} // namespace Fixtures
} // namespace Test::ZipDirFs::Zip

#endif // TEST_ZIPDIRFS_ZIP_FIXTURES_STREAMBUFFERINITIALIZER_H
