/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "ZipDirFs/Zip/Stream.h"
#include "ZipDirFs/Zip/Archive.h"
#include "ZipDirFs/Zip/Entry.h"
#include "ZipDirFs/Zip/Factory.h"
#include "ZipDirFs/Zip/Lib.h"
#include <boost/filesystem.hpp>

namespace ZipDirFs::Zip
{
	namespace
	{
		class StreamBuffer : public std::basic_streambuf<char>
		{
		public:
			StreamBuffer(const std::shared_ptr<Base::Content>&);
			~StreamBuffer();

		protected:
			pos_type seekoff(off_type, std::ios_base::seekdir, std::ios_base::openmode);
			pos_type seekpos(pos_type, std::ios_base::openmode);
			std::streamsize showmanyc();
			int_type underflow();
			std::streamsize xsgetn(char_type*, std::streamsize);
			const std::shared_ptr<Base::Content> content;
		};
		const std::streamsize chunksize = 4096;
	} // namespace

	Stream::Stream(const boost::filesystem::path& p, const std::string& n) :
		Stream(*Factory::getInstance().get(p), n)
	{
	}
	Stream::Stream(Archive& a, const std::string& n) : Stream(*a.open(n)) {}
	// TODO: Select direct access streambuffer if not compressed.
	Stream::Stream(Entry& e) : std::istream(createBuffer(e.open())) {}
	Stream::~Stream()
	{
		if (rdbuf() != nullptr)
			delete rdbuf();
	}
	std::basic_streambuf<Stream::char_type>* Stream::createBuffer(
		const std::shared_ptr<Base::Content>& c)
	{
		return new StreamBuffer(c);
	}

	void ZipReadChunk(Base::Content&, Base::Content::lock&);
	void ZipEnsureDataAvailableUntil(Base::Content&, StreamBuffer::char_type*);
	StreamBuffer::StreamBuffer(const std::shared_ptr<Base::Content>& c) : content(c)
	{
		setg(c->buffer, c->buffer, c->buffer + c->lastWrite);
	}
	StreamBuffer::~StreamBuffer() {}
	StreamBuffer::pos_type StreamBuffer::seekoff(
		StreamBuffer::off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which)
	{
		auto position = gptr();
		if ((which & std::ios_base::openmode::_S_in) != 0)
		{
			auto end = content->buffer + content->length;
			off = std::max(-content->length, std::min(content->length, off));
			switch (dir)
			{
			case std::ios_base::seekdir::_S_beg:
				position = content->buffer + off;
				break;
			case std::ios_base::seekdir::_S_end:
				position = end + off;
				break;
			case std::ios_base::seekdir::_S_cur:
				position += off;
				break;
			}
			position = std::max(std::min(position, end), content->buffer);
			ZipEnsureDataAvailableUntil(*content, position);
			setg(position, position, position);
			return position - content->buffer;
		}
		return std::streambuf::seekoff(off, dir, which);
	}
	StreamBuffer::pos_type StreamBuffer::seekpos(
		StreamBuffer::pos_type pos, std::ios_base::openmode which)
	{
		if ((which & std::ios_base::openmode::_S_in) != 0)
		{
			auto position = content->buffer + std::min<pos_type>(pos, content->length);
			ZipEnsureDataAvailableUntil(*content, position);
			setg(position, position, position);
			return position - content->buffer;
		}
		return std::streambuf::seekpos(pos, which);
	}
	std::streamsize StreamBuffer::showmanyc()
	{
		return content->length - (gptr() - content->buffer);
	}
	StreamBuffer::int_type StreamBuffer::underflow()
	{
		auto lock(content->readLock());
		auto current = gptr();
		if (current < content->buffer + content->length)
		{
			auto last = content->buffer + content->lastWrite;
			if (last == current)
			{
				ZipReadChunk(*content, lock);
			}
			setg(content->buffer, gptr(), content->buffer + content->lastWrite);
			return *gptr();
		}
		return traits_type::eof();
	}
	std::streamsize StreamBuffer::xsgetn(StreamBuffer::char_type* dest, std::streamsize len)
	{
		auto source = gptr();
		auto end = std::min(source + len, content->buffer + content->length);
		len = end - source;
		ZipEnsureDataAvailableUntil(*content, end);
		std::memcpy(dest, source, len);
		return len;
	}
	void ZipEnsureDataAvailableUntil(Base::Content& content, StreamBuffer::char_type* position)
	{
		auto lock(content.readLock());
		while (position > content.buffer + content.lastWrite)
		{
			ZipReadChunk(content, lock);
		}
	}
	void ZipReadChunk(Base::Content& content, Base::Content::lock& lock)
	{
		auto lastWrite = content.lastWrite;
		lock.makeWriter();
		auto& c = content;
		if (lastWrite == c.lastWrite && c.data != nullptr)
		{
			auto len = Lib::fread(c.data, c.buffer + lastWrite, chunksize);
			c.lastWrite += len;
			if (c.lastWrite == c.length)
			{
				Lib::fclose(c.data);
				c.data = nullptr;
			}
		}
		lock.makeReader();
	}
} // namespace ZipDirFs::Zip
