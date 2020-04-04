/*
 * Copyright © 2020 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "StreamBufferInitializer.h"

namespace Test::ZipDirFs::Zip
{
	namespace Fixtures
	{
		std::basic_streambuf<::ZipDirFs::Zip::Stream::char_type>* StreamBufferInitializer::create(
			const std::shared_ptr<::ZipDirFs::Zip::Base::Content>& c)
		{
			return ::ZipDirFs::Zip::Stream::createBuffer(c);
		}
		std::shared_ptr<::ZipDirFs::Zip::Base::Content> StreamBufferInitializer::getContent(
			std::basic_streambuf<::ZipDirFs::Zip::Stream::char_type>* b)
		{
			return reinterpret_cast<StreamBufferInitializer*>(b)->content;
		}
		void StreamBufferInitializer::invokeSetg(
			std::basic_streambuf<char_type>* b, char_type* gbeg, char_type* gcurr, char_type* gend)
		{
			reinterpret_cast<StreamBufferInitializer*>(b)->setg(gbeg, gcurr, gend);
		}
		StreamBufferInitializer::char_type* StreamBufferInitializer::invokeEback(
			const std::basic_streambuf<char_type>* b)
		{
			return reinterpret_cast<const StreamBufferInitializer*>(b)->eback();
		}
		StreamBufferInitializer::char_type* StreamBufferInitializer::invokeGptr(
			const std::basic_streambuf<char_type>* b)
		{
			return reinterpret_cast<const StreamBufferInitializer*>(b)->gptr();
		}
		StreamBufferInitializer::char_type* StreamBufferInitializer::invokeEgptr(
			const std::basic_streambuf<char_type>* b)
		{
			return reinterpret_cast<const StreamBufferInitializer*>(b)->egptr();
		}
		std::streamsize StreamBufferInitializer::invokeShowmanyc(std::basic_streambuf<char_type>* b)
		{
			return reinterpret_cast<StreamBufferInitializer*>(b)->showmanyc();
		}
		StreamBufferInitializer::int_type StreamBufferInitializer::invokeUnderflow(
			std::basic_streambuf<char_type>* b)
		{
			return reinterpret_cast<StreamBufferInitializer*>(b)->underflow();
		}
		std::streamsize StreamBufferInitializer::invokeXsgetn(
			std::basic_streambuf<char_type>* b, char_type* d, std::streamsize l)
		{
			return reinterpret_cast<StreamBufferInitializer*>(b)->xsgetn(d, l);
		}
	} // namespace Fixtures
} // namespace Test::ZipDirFs::Zip
