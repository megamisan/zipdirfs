/*
 * Copyright © 2020-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#define BUILD_TEST
#include "Stream.h"
#include "Fixtures/Lib.h"
#include "Fixtures/SimpleSpan.h"
#include "Fixtures/StreamBufferInitializer.h"
#include "ZipDirFs/Zip/Stream.h"
#include "test/gtest.h"
#include <algorithm>
#include <array>
#include <csignal>
#include <gmock/gmock.h>

namespace Test::ZipDirFs::Zip
{
	using Fixtures::StreamBufferInitializer;
	using ::testing::_;
	using ::testing::Invoke;
	using ::testing::Return;
	using ::testing::Throw;
	using ::ZipDirFs::Zip::Stream;
	using ::ZipDirFs::Zip::Base::Content;
	template <Fixtures::SimpleSpanImpl::size_type size>
	using SimpleSpan = Fixtures::SimpleSpan<Stream::char_type, size>;

	namespace
	{
		typedef std::basic_streambuf<Stream::char_type> streambuf;
		struct Appendable
		{
			constexpr Appendable(
				std::add_const<void*>::type d, std::add_const<std::size_t>::type l) :
				dest(d),
				len(l)
			{
			}
			Appendable append(void* s, std::size_t l)
			{
				l = std::min(l, len);
				return Appendable((char*)memcpy(dest, s, l) + l, len - l);
			}

		private:
			std::add_const<void*>::type dest;
			std::add_const<std::size_t>::type len;
		};
	} // namespace

	::ZipDirFs::Zip::Base::Content& StreamAccess::getStreamBufferContent(Stream& s)
	{
		return StreamBufferAccess::getContent(s.rdbuf());
	}
	streambuf*& StreamAccess::getStreamStreamBuffer(Stream& s)
	{
		return reinterpret_cast<StreamAccess*>(&s)->_M_streambuf;
	}

	StreamAccess::GlobalHelper::GlobalHelper(std::uint64_t length) :
		data(reinterpret_cast<decltype(data)>(::Test::rand(INT32_MAX))), lib(),
		path("archive" + std::to_string(::Test::rand(INT32_MAX))),
		name("file" + std::to_string(::Test::rand(INT32_MAX))), archiveInstance(data),
		archive(std::shared_ptr<::ZipDirFs::Zip::Archive>(
			&archiveInstance, [](::ZipDirFs::Zip::Archive*) {})),
		entryInstance(std::shared_ptr<LibBase>(archive, data), name, false),
		entry(std::shared_ptr<::ZipDirFs::Zip::Entry>(&entryInstance,
			[this](::ZipDirFs::Zip::Entry*)
			{
				auto& content = EntryAccess::getContent(entryInstance);
				delete[] content.buffer;
				content.buffer = nullptr;
				content.data = nullptr;
			}))
	{
		std::uint64_t index(::Test::rand(INT32_MAX));
		FactoryAccess::getArchivesByPath().emplace(path, archive);
		FactoryAccess::getArchivesByData().emplace(data, archive);
		ArchiveAccess::getNames(archiveInstance).emplace_back(name);
		ArchiveAccess::getNameAttributes(archiveInstance)
			.emplace(name, std::tuple<std::uint64_t, bool>{index, false});
		ArchiveAccess::getEntries(archiveInstance).emplace(index, entry);
		auto& content = EntryAccess::getContent(entryInstance);
		content.buffer = new char[length];
		content.length = length;
		content.data = reinterpret_cast<decltype(content.data)>(::Test::rand(INT32_MAX));
		EXPECT_CALL(lib, close(_));
	}
	StreamAccess::GlobalHelper::~GlobalHelper()
	{
		FactoryAccess::getArchivesByPath().clear();
		FactoryAccess::getArchivesByData().clear();
	}
	::ZipDirFs::Zip::Archive& StreamAccess::GlobalHelper::getArchive() { return archiveInstance; }
	::ZipDirFs::Zip::Entry& StreamAccess::GlobalHelper::getEntry() { return entryInstance; }
	const boost::filesystem::path& StreamAccess::GlobalHelper::getPath() { return path; }
	const std::string& StreamAccess::GlobalHelper::getName() { return name; }
	Fixtures::Lib& StreamAccess::GlobalHelper::getLib() { return lib; }
	Content& StreamAccess::GlobalHelper::getEntryContent()
	{
		return EntryAccess::getContent(entryInstance);
	}

	Content& StreamAccess::StreamBufferAccess::getContent(streambuf* sb)
	{
		return *reinterpret_cast<StreamBufferAccess*>(sb)->content;
	}

	Content& StreamAccess::EntryAccess::getContent(::ZipDirFs::Zip::Entry& e)
	{
		return reinterpret_cast<EntryAccess*>(&e)->content;
	}

	std::vector<std::string>& StreamAccess::ArchiveAccess::getNames(::ZipDirFs::Zip::Archive& a)
	{
		return reinterpret_cast<ArchiveAccess*>(&a)->names;
	}
	std::map<std::string, std::tuple<std::uint64_t, bool>>&
		StreamAccess::ArchiveAccess::getNameAttributes(::ZipDirFs::Zip::Archive& a)
	{
		return reinterpret_cast<ArchiveAccess*>(&a)->nameAttributes;
	}
	std::map<std::uint64_t, std::weak_ptr<::ZipDirFs::Zip::Entry>>&
		StreamAccess::ArchiveAccess::getEntries(::ZipDirFs::Zip::Archive& a)
	{
		return reinterpret_cast<ArchiveAccess*>(&a)->entries;
	}

	std::map<const ::ZipDirFs::Zip::Base::Lib*, std::weak_ptr<::ZipDirFs::Zip::Archive>>&
		StreamAccess::FactoryAccess::getArchivesByData()
	{
		return Factory::archivesByData;
	}
	std::map<boost::filesystem::path, std::weak_ptr<::ZipDirFs::Zip::Archive>>&
		StreamAccess::FactoryAccess::getArchivesByPath()
	{
		return Factory::archivesByPath;
	}

	void StreamBufferTest::SetUp()
	{
		content = std::shared_ptr<Content>(new Content);
		buffer = std::shared_ptr<streambuf>(StreamBufferInitializer::create(content));
	}

	void StreamBufferTest::TearDown()
	{
		buffer = nullptr;
		content = nullptr;
	}

	void StreamBufferTest::GenerateRandomCompletedContentState()
	{
		content->buffer = reinterpret_cast<decltype(content->buffer)>(::Test::rand(INT32_MAX));
		content->length = content->lastWrite = ::Test::rand(randomLenBase(), randomLenMax());
	}

	std::unique_ptr<Stream::char_type[]> StreamBufferTest::GenerateRandomBufferedContentState()
	{
		content->length = ::Test::rand(randomLenBase(), randomLenMax() - 1);
		content->lastWrite = ::Test::rand(long(randomLenBase()), content->length);
		std::unique_ptr<Stream::char_type[]> buffer(new Stream::char_type[content->length]);
		GenerateRandomSegment(content->buffer = buffer.get(), content->lastWrite);
		return buffer;
	}

	void StreamBufferTest::GenerateRandomSegment(Stream::char_type* buffer, std::streamsize len)
	{
		using rand_type = double;
		const std::streamsize last = len - (len % sizeof(rand_type));
		rand_type* dest = reinterpret_cast<decltype(dest)>(buffer);
		rand_type* end = reinterpret_cast<decltype(end)>(buffer) + last / sizeof(rand_type);
		while (dest < end)
		{
			*(dest++) = ::Test::rand();
		}
		if (last != len)
		{
			rand_type value = ::Test::rand();
			std::memcpy(dest, &value, len - last);
		}
	}

	constexpr std::uint64_t StreamBufferTest::randomLenModulo() { return 1048576; }
	constexpr std::uint64_t StreamBufferTest::randomLenBase() { return 4096; }
	constexpr std::uint64_t StreamBufferTest::randomLenMax()
	{
		return randomLenModulo() + randomLenBase() - 1;
	}

	TEST_F(StreamTest, InstantiatePath)
	{
		StreamAccess::GlobalHelper helper(0);
		Stream stream(helper.getPath(), helper.getName());
		EXPECT_EQ(&StreamAccess::getStreamBufferContent(stream), &helper.getEntryContent());
	}

	TEST_F(StreamTest, InstantiateArchive)
	{
		StreamAccess::GlobalHelper helper(0);
		Stream stream(helper.getArchive(), helper.getName());
		EXPECT_EQ(&StreamAccess::getStreamBufferContent(stream), &helper.getEntryContent());
	}

	TEST_F(StreamTest, InstantiateEntry)
	{
		StreamAccess::GlobalHelper helper(0);
		Stream stream(helper.getEntry());
		EXPECT_EQ(&StreamAccess::getStreamBufferContent(stream), &helper.getEntryContent());
	}

	TEST_F(StreamTest, InstantiateCoverage)
	{
		StreamAccess::GlobalHelper helper(0);
		std::unique_ptr<Stream>(new Stream(helper.getPath(), helper.getName()));
		std::unique_ptr<Stream>(new Stream(helper.getArchive(), helper.getName()));
		std::unique_ptr<Stream>(new Stream(helper.getEntry()));
	}

	TEST_F(StreamTest, Destroy)
	{
		EXPECT_EXIT(
			{
				char c[0];
				StreamAccess::GlobalHelper helper(0);
				Stream stream(helper.getEntry());
				delete StreamAccess::getStreamStreamBuffer(stream);
				StreamAccess::getStreamStreamBuffer(stream) = reinterpret_cast<streambuf*>(c);
			},
			::testing::KilledBySignal(SIGSEGV), "");
	}

	TEST_F(StreamBufferTest, InstantiateNull)
	{
		EXPECT_EXIT(
			{
				std::shared_ptr<streambuf> buf(
					StreamBufferInitializer::create(std::shared_ptr<Content>()));
			},
			::testing::KilledBySignal(SIGSEGV), "");
	}

	TEST_F(StreamBufferTest, InstantiateSet)
	{
		content->buffer = reinterpret_cast<decltype(content->buffer)>(::Test::rand(INT32_MAX));
		content->lastWrite = ::Test::rand(randomLenBase(), randomLenMax());
		buffer = std::shared_ptr<streambuf>(StreamBufferInitializer::create(content));
		EXPECT_EQ(StreamBufferInitializer::getContent(buffer.get()), content);
		EXPECT_EQ(StreamBufferInitializer::invokeGptr(buffer.get()), content->buffer);
		EXPECT_EQ(StreamBufferInitializer::invokeEback(buffer.get()), content->buffer);
		EXPECT_EQ(StreamBufferInitializer::invokeEgptr(buffer.get()),
			content->buffer + content->lastWrite);
	}

	TEST_F(StreamBufferTest, SeekOffOutBeg0)
	{
		std::array<Stream::char_type*, 3> pointers = {
			reinterpret_cast<Stream::char_type*>(::Test::rand(INT32_MAX)),
			reinterpret_cast<Stream::char_type*>(::Test::rand(INT32_MAX)),
			reinterpret_cast<Stream::char_type*>(::Test::rand(INT32_MAX))};
		std::sort(begin(pointers), end(pointers));
		StreamBufferInitializer::invokeSetg(buffer.get(), pointers[0], pointers[1], pointers[2]);
		buffer->pubseekoff(0, std::ios_base::seekdir::_S_beg, std::ios_base::openmode::_S_out);
		EXPECT_EQ(StreamBufferInitializer::invokeEback(buffer.get()), pointers[0]);
		EXPECT_EQ(StreamBufferInitializer::invokeGptr(buffer.get()), pointers[1]);
		EXPECT_EQ(StreamBufferInitializer::invokeEgptr(buffer.get()), pointers[2]);
	}

	TEST_F(StreamBufferTest, SeekOffInBeg)
	{
		GenerateRandomCompletedContentState();
		StreamBufferInitializer::invokeSetg(buffer.get(), content->buffer,
			content->buffer + ::Test::rand(content->length), content->buffer + content->length);
		std::streamoff newpos = ::Test::rand(content->length);
		Stream::char_type* expected = content->buffer + newpos;
		EXPECT_EQ(buffer->pubseekoff(
					  newpos, std::ios_base::seekdir::_S_beg, std::ios_base::openmode::_S_in),
			newpos);
		EXPECT_EQ(StreamBufferInitializer::invokeEback(buffer.get()), expected);
		EXPECT_EQ(StreamBufferInitializer::invokeGptr(buffer.get()), expected);
		EXPECT_EQ(StreamBufferInitializer::invokeEgptr(buffer.get()), expected);
	}

	TEST_F(StreamBufferTest, SeekOffInBegAfterEnd)
	{
		GenerateRandomCompletedContentState();
		StreamBufferInitializer::invokeSetg(buffer.get(), content->buffer,
			content->buffer + ::Test::rand(content->length), content->buffer + content->length);
		std::streamoff newpos = content->length + ::Test::rand(randomLenBase());
		Stream::char_type* expected = content->buffer + content->length;
		EXPECT_EQ(buffer->pubseekoff(
					  newpos, std::ios_base::seekdir::_S_beg, std::ios_base::openmode::_S_in),
			content->length);
		EXPECT_EQ(StreamBufferInitializer::invokeEback(buffer.get()), expected);
		EXPECT_EQ(StreamBufferInitializer::invokeGptr(buffer.get()), expected);
		EXPECT_EQ(StreamBufferInitializer::invokeEgptr(buffer.get()), expected);
	}

	TEST_F(StreamBufferTest, SeekOffInCur)
	{
		GenerateRandomCompletedContentState();
		Stream::char_type* start =
			content->buffer + (content->length >> 2) + (::Test::rand(content->length) >> 1);
		StreamBufferInitializer::invokeSetg(
			buffer.get(), content->buffer, start, content->buffer + content->length);
		std::streamoff newpos = (::Test::rand(content->length) >> 1) - (content->length >> 2);
		Stream::char_type* expected = start + newpos;
		std::streamoff expectedPos = expected - content->buffer;
		EXPECT_EQ(buffer->pubseekoff(
					  newpos, std::ios_base::seekdir::_S_cur, std::ios_base::openmode::_S_in),
			expectedPos);
		EXPECT_EQ(StreamBufferInitializer::invokeEback(buffer.get()), expected);
		EXPECT_EQ(StreamBufferInitializer::invokeGptr(buffer.get()), expected);
		EXPECT_EQ(StreamBufferInitializer::invokeEgptr(buffer.get()), expected);
	}

	TEST_F(StreamBufferTest, SeekOffInCurBeforeBeg)
	{
		GenerateRandomCompletedContentState();
		Stream::char_type* start = content->buffer + (content->length >> 1);
		StreamBufferInitializer::invokeSetg(
			buffer.get(), content->buffer, start, content->buffer + content->length);
		std::streamoff newpos = -content->length;
		Stream::char_type* expected = content->buffer;
		EXPECT_EQ(buffer->pubseekoff(
					  newpos, std::ios_base::seekdir::_S_cur, std::ios_base::openmode::_S_in),
			0);
		EXPECT_EQ(StreamBufferInitializer::invokeEback(buffer.get()), expected);
		EXPECT_EQ(StreamBufferInitializer::invokeGptr(buffer.get()), expected);
		EXPECT_EQ(StreamBufferInitializer::invokeEgptr(buffer.get()), expected);
	}

	TEST_F(StreamBufferTest, SeekOffInCurAfterEnd)
	{
		GenerateRandomCompletedContentState();
		Stream::char_type* start = content->buffer + (content->length >> 1);
		StreamBufferInitializer::invokeSetg(
			buffer.get(), content->buffer, start, content->buffer + content->length);
		std::streamoff newpos = content->length;
		Stream::char_type* expected = content->buffer + content->length;
		EXPECT_EQ(buffer->pubseekoff(
					  newpos, std::ios_base::seekdir::_S_cur, std::ios_base::openmode::_S_in),
			content->length);
		EXPECT_EQ(StreamBufferInitializer::invokeEback(buffer.get()), expected);
		EXPECT_EQ(StreamBufferInitializer::invokeGptr(buffer.get()), expected);
		EXPECT_EQ(StreamBufferInitializer::invokeEgptr(buffer.get()), expected);
	}

	TEST_F(StreamBufferTest, SeekOffInEnd)
	{
		GenerateRandomCompletedContentState();
		StreamBufferInitializer::invokeSetg(buffer.get(), content->buffer,
			content->buffer + ::Test::rand(content->length), content->buffer + content->length);
		std::streamoff newpos = ::Test::rand(std::streamoff(-content->length), std::streamoff(-1));
		Stream::char_type* expected = content->buffer + content->length + newpos;
		EXPECT_EQ(buffer->pubseekoff(
					  newpos, std::ios_base::seekdir::_S_end, std::ios_base::openmode::_S_in),
			content->length + newpos);
		EXPECT_EQ(StreamBufferInitializer::invokeEback(buffer.get()), expected);
		EXPECT_EQ(StreamBufferInitializer::invokeGptr(buffer.get()), expected);
		EXPECT_EQ(StreamBufferInitializer::invokeEgptr(buffer.get()), expected);
	}

	TEST_F(StreamBufferTest, SeekOffInEndBeforeBeg)
	{
		GenerateRandomCompletedContentState();
		StreamBufferInitializer::invokeSetg(buffer.get(), content->buffer,
			content->buffer + ::Test::rand(content->length), content->buffer + content->length);
		std::streamoff newpos = ::Test::rand(
			std::streamoff(-content->length - randomLenBase()), std::streamoff(-content->length));
		Stream::char_type* expected = content->buffer;
		EXPECT_EQ(buffer->pubseekoff(
					  newpos, std::ios_base::seekdir::_S_end, std::ios_base::openmode::_S_in),
			0);
		EXPECT_EQ(StreamBufferInitializer::invokeEback(buffer.get()), expected);
		EXPECT_EQ(StreamBufferInitializer::invokeGptr(buffer.get()), expected);
		EXPECT_EQ(StreamBufferInitializer::invokeEgptr(buffer.get()), expected);
	}

	TEST_F(StreamBufferTest, SeekPosOut0)
	{
		std::array<Stream::char_type*, 3> pointers = {
			reinterpret_cast<Stream::char_type*>(::Test::rand(INT32_MAX)),
			reinterpret_cast<Stream::char_type*>(::Test::rand(INT32_MAX)),
			reinterpret_cast<Stream::char_type*>(::Test::rand(INT32_MAX))};
		std::sort(begin(pointers), end(pointers));
		StreamBufferInitializer::invokeSetg(buffer.get(), pointers[0], pointers[1], pointers[2]);
		buffer->pubseekpos(0, std::ios_base::openmode::_S_out);
		EXPECT_EQ(StreamBufferInitializer::invokeEback(buffer.get()), pointers[0]);
		EXPECT_EQ(StreamBufferInitializer::invokeGptr(buffer.get()), pointers[1]);
		EXPECT_EQ(StreamBufferInitializer::invokeEgptr(buffer.get()), pointers[2]);
	}

	TEST_F(StreamBufferTest, SeekPosIn)
	{
		GenerateRandomCompletedContentState();
		StreamBufferInitializer::invokeSetg(buffer.get(), content->buffer,
			content->buffer + ::Test::rand(content->length), content->buffer + content->length);
		std::streamoff newpos = ::Test::rand(content->length);
		Stream::char_type* expected = content->buffer + newpos;
		EXPECT_EQ(buffer->pubseekpos(newpos, std::ios_base::openmode::_S_in), newpos);
		EXPECT_EQ(StreamBufferInitializer::invokeEback(buffer.get()), expected);
		EXPECT_EQ(StreamBufferInitializer::invokeGptr(buffer.get()), expected);
		EXPECT_EQ(StreamBufferInitializer::invokeEgptr(buffer.get()), expected);
	}

	TEST_F(StreamBufferTest, SeekPosInAfterEnd)
	{
		GenerateRandomCompletedContentState();
		StreamBufferInitializer::invokeSetg(buffer.get(), content->buffer,
			content->buffer + ::Test::rand(content->length), content->buffer + content->length);
		std::streamoff newpos = content->length + ::Test::rand(randomLenBase());
		Stream::char_type* expected = content->buffer + content->length;
		EXPECT_EQ(buffer->pubseekpos(newpos, std::ios_base::openmode::_S_in), content->length);
		EXPECT_EQ(StreamBufferInitializer::invokeEback(buffer.get()), expected);
		EXPECT_EQ(StreamBufferInitializer::invokeGptr(buffer.get()), expected);
		EXPECT_EQ(StreamBufferInitializer::invokeEgptr(buffer.get()), expected);
	}

	TEST_F(StreamBufferTest, SHowManyC)
	{
		GenerateRandomCompletedContentState();
		std::streamsize position = ::Test::rand(content->length);
		StreamBufferInitializer::invokeSetg(buffer.get(), content->buffer,
			content->buffer + position, content->buffer + content->length);
		EXPECT_EQ(
			StreamBufferInitializer::invokeShowmanyc(buffer.get()), content->length - position);
	}

	TEST_F(StreamBufferTest, UnderflowAtEnd)
	{
		GenerateRandomCompletedContentState();
		StreamBufferInitializer::invokeSetg(buffer.get(), content->buffer,
			content->buffer + content->length, content->buffer + content->length);
		const auto eof = StreamBufferInitializer::traits_type::eof();
		EXPECT_EQ(StreamBufferInitializer::invokeUnderflow(buffer.get()), eof);
	}

	TEST_F(StreamBufferTest, UnderflowAlreadyAvailable)
	{
		auto bufferPtr = GenerateRandomBufferedContentState();
		StreamBufferInitializer::invokeSetg(buffer.get(), content->buffer,
			content->buffer + content->lastWrite - 1, content->buffer + content->lastWrite);
		EXPECT_EQ(StreamBufferInitializer::invokeUnderflow(buffer.get()),
			content->buffer[content->lastWrite - 1]);
		EXPECT_EQ(StreamBufferInitializer::invokeEback(buffer.get()), content->buffer);
		EXPECT_EQ(StreamBufferInitializer::invokeGptr(buffer.get()),
			content->buffer + content->lastWrite - 1);
		EXPECT_EQ(StreamBufferInitializer::invokeEgptr(buffer.get()),
			content->buffer + content->lastWrite);
	}

	TEST_F(StreamBufferTest, UnderflowNotYetAvailableFull)
	{
		Fixtures::Lib lib;
		::ZipDirFs::Zip::Base::Lib::File* data =
			reinterpret_cast<decltype(data)>(::Test::rand(UINT32_MAX));
		char random[randomLenBase()];
		GenerateRandomSegment(random, sizeof(random));
		auto bufferPtr = GenerateRandomBufferedContentState();
		auto expectedPosition = content->lastWrite;
		StreamBufferInitializer::invokeSetg(buffer.get(), content->buffer,
			content->buffer + content->lastWrite, content->buffer + content->lastWrite);
		content->data = data;
		EXPECT_CALL(lib, fread(data, content->buffer + content->lastWrite, randomLenBase()))
			.WillOnce(Invoke(
				[&random](decltype(data) d, void* b, std::uint64_t l)
				{
					memcpy(b, random, l);
					return sizeof(random);
				}));
		EXPECT_EQ(StreamBufferInitializer::invokeUnderflow(buffer.get()), random[0]);
		EXPECT_EQ(SimpleSpan<randomLenBase()>(random).content(),
			SimpleSpan<randomLenBase()>(content->buffer + expectedPosition).content());
		EXPECT_EQ(StreamBufferInitializer::invokeEback(buffer.get()), content->buffer);
		EXPECT_EQ(
			StreamBufferInitializer::invokeGptr(buffer.get()), content->buffer + expectedPosition);
		EXPECT_EQ(StreamBufferInitializer::invokeEgptr(buffer.get()),
			content->buffer + expectedPosition + randomLenBase());
	}

	TEST_F(StreamBufferTest, UnderflowNotYetAvailablePartial)
	{
		using Fixtures::dynamic_extend;
		Fixtures::Lib lib;
		::ZipDirFs::Zip::Base::Lib::File* data =
			reinterpret_cast<decltype(data)>(::Test::rand(UINT32_MAX));
		std::uint64_t length = (randomLenBase() >> 2) + (::Test::rand(randomLenBase()) >> 1);
		std::unique_ptr<char[]> randomPtr(new char[length]);
		GenerateRandomSegment(randomPtr.get(), length);
		auto bufferPtr = GenerateRandomBufferedContentState();
		content->lastWrite =
			std::min(content->lastWrite, std::streamsize(content->length - length));
		auto expectedPosition = content->lastWrite;
		StreamBufferInitializer::invokeSetg(buffer.get(), content->buffer,
			content->buffer + expectedPosition, content->buffer + expectedPosition);
		content->data = data;
		EXPECT_CALL(lib, fread(data, content->buffer + expectedPosition, randomLenBase()))
			.WillOnce(Invoke(
				[&randomPtr, length](decltype(data) d, void* b, std::uint64_t l)
				{
					memcpy(b, randomPtr.get(), length);
					return length;
				}));
		EXPECT_EQ(StreamBufferInitializer::invokeUnderflow(buffer.get()), randomPtr[0]);
		EXPECT_EQ(SimpleSpan<dynamic_extend>(randomPtr.get(), length).content(),
			SimpleSpan<dynamic_extend>(content->buffer + expectedPosition, length).content());
		EXPECT_EQ(StreamBufferInitializer::invokeEback(buffer.get()), content->buffer);
		EXPECT_EQ(
			StreamBufferInitializer::invokeGptr(buffer.get()), content->buffer + expectedPosition);
		EXPECT_EQ(StreamBufferInitializer::invokeEgptr(buffer.get()),
			content->buffer + expectedPosition + length);
	}

	TEST_F(StreamBufferTest, XSGetNAlreadyAvailable)
	{
		using Fixtures::dynamic_extend;
		auto bufferPtr = GenerateRandomBufferedContentState();
		std::uint64_t position = ::Test::rand(content->lastWrite - randomLenBase() - 1);
		std::uint64_t length = (randomLenBase() >> 2) + (::Test::rand(randomLenBase()) >> 1);
		std::unique_ptr<char[]> resultPtr(new char[length]);
		StreamBufferInitializer::invokeSetg(buffer.get(), content->buffer,
			content->buffer + position, content->buffer + content->lastWrite);
		std::uint64_t expectedRead = std::min(length, content->length - position);
		EXPECT_EQ(StreamBufferInitializer::invokeXsgetn(buffer.get(), resultPtr.get(), length),
			expectedRead);
		EXPECT_EQ(SimpleSpan<dynamic_extend>(resultPtr.get(), length).content(),
			SimpleSpan<dynamic_extend>(content->buffer + position, length).content());
		EXPECT_EQ(StreamBufferInitializer::invokeEback(buffer.get()), content->buffer);
		EXPECT_EQ(StreamBufferInitializer::invokeGptr(buffer.get()), content->buffer + position);
		EXPECT_EQ(StreamBufferInitializer::invokeEgptr(buffer.get()),
			content->buffer + content->lastWrite);
	}

	TEST_F(StreamBufferTest, XSGetNNotYetAvailableOneChunk)
	{
		using Fixtures::dynamic_extend;
		Fixtures::Lib lib;
		::ZipDirFs::Zip::Base::Lib::File* data =
			reinterpret_cast<decltype(data)>(::Test::rand(UINT32_MAX));
		auto bufferPtr = GenerateRandomBufferedContentState();
		std::uint64_t position = content->lastWrite - ((::Test::rand(randomLenBase() - 4)) >> 2);
		std::uint64_t length = (randomLenBase() >> 2) + (::Test::rand(randomLenBase()) >> 2);
		std::unique_ptr<char[]> expectedPtr(new char[length]);
		std::unique_ptr<char[]> resultPtr(new char[length]);
		char random[randomLenBase() >> 1];
		GenerateRandomSegment(random, sizeof(random));
		Appendable(expectedPtr.get(), length)
			.append(content->buffer + position, content->lastWrite - position)
			.append(random, sizeof(random));
		auto lastWrite = content->lastWrite;
		StreamBufferInitializer::invokeSetg(
			buffer.get(), content->buffer, content->buffer + position, content->buffer + lastWrite);
		content->data = data;
		EXPECT_CALL(lib, fread(data, content->buffer + lastWrite, randomLenBase()))
			.WillOnce(Invoke(
				[&random](decltype(data) d, void* b, std::uint64_t l)
				{
					memcpy(b, random, l);
					return sizeof(random);
				}));
		EXPECT_EQ(
			StreamBufferInitializer::invokeXsgetn(buffer.get(), resultPtr.get(), length), length);
		EXPECT_EQ(SimpleSpan<dynamic_extend>(resultPtr.get(), length).content(),
			SimpleSpan<dynamic_extend>(expectedPtr.get(), length).content());
		EXPECT_EQ(StreamBufferInitializer::invokeEback(buffer.get()), content->buffer);
		EXPECT_EQ(StreamBufferInitializer::invokeGptr(buffer.get()), content->buffer + position);
		EXPECT_EQ(StreamBufferInitializer::invokeEgptr(buffer.get()), content->buffer + lastWrite);
	}

	TEST_F(StreamBufferTest, XSGetNNotYetAvailableTwoChunks)
	{
		using Fixtures::dynamic_extend;
		Fixtures::Lib lib;
		::ZipDirFs::Zip::Base::Lib::File* data =
			reinterpret_cast<decltype(data)>(::Test::rand(UINT32_MAX));
		auto bufferPtr = GenerateRandomBufferedContentState();
		std::uint64_t position = content->lastWrite - (::Test::rand(randomLenBase() - 4) >> 2);
		std::uint64_t length =
			(randomLenBase() >> 1) + (randomLenBase() >> 2) + (::Test::rand(randomLenBase()) >> 2);
		std::unique_ptr<char[]> expectedPtr(new char[length]);
		std::unique_ptr<char[]> resultPtr(new char[length]);
		char random1[randomLenBase() >> 1];
		char random2[randomLenBase() >> 1];
		GenerateRandomSegment(random1, sizeof(random1));
		GenerateRandomSegment(random2, sizeof(random2));
		Appendable(expectedPtr.get(), length)
			.append(content->buffer + position, content->lastWrite - position)
			.append(random1, sizeof(random1))
			.append(random2, sizeof(random2));
		auto lastWrite = content->lastWrite;
		StreamBufferInitializer::invokeSetg(
			buffer.get(), content->buffer, content->buffer + position, content->buffer + lastWrite);
		content->data = data;
		EXPECT_CALL(lib, fread(data, content->buffer + lastWrite, randomLenBase()))
			.WillOnce(Invoke(
				[&random1](decltype(data) d, void* b, std::uint64_t l)
				{
					memcpy(b, random1, l);
					return sizeof(random1);
				}));
		EXPECT_CALL(
			lib, fread(data, content->buffer + lastWrite + (randomLenBase() >> 1), randomLenBase()))
			.WillOnce(Invoke(
				[&random2](decltype(data) d, void* b, std::uint64_t l)
				{
					memcpy(b, random2, l);
					return sizeof(random2);
				}));
		EXPECT_EQ(
			StreamBufferInitializer::invokeXsgetn(buffer.get(), resultPtr.get(), length), length);
		EXPECT_EQ(SimpleSpan<dynamic_extend>(resultPtr.get(), length).content(),
			SimpleSpan<dynamic_extend>(expectedPtr.get(), length).content());
		EXPECT_EQ(StreamBufferInitializer::invokeEback(buffer.get()), content->buffer);
		EXPECT_EQ(StreamBufferInitializer::invokeGptr(buffer.get()), content->buffer + position);
		EXPECT_EQ(StreamBufferInitializer::invokeEgptr(buffer.get()), content->buffer + lastWrite);
	}

	TEST_F(StreamBufferTest, LastRead)
	{
		Fixtures::Lib lib;
		::ZipDirFs::Zip::Base::Lib::File* data =
			reinterpret_cast<decltype(data)>(::Test::rand(UINT32_MAX));
		content->length = 1;
		content->lastWrite = 0;
		char val = 'a';
		content->buffer = &val;
		content->data = data;
		buffer = std::shared_ptr<streambuf>(StreamBufferInitializer::create(content));
		::testing::ExpectationSet set;
		set += EXPECT_CALL(lib, fread(data, content->buffer, randomLenBase())).WillOnce(Return(1));
		EXPECT_CALL(lib, fclose(data)).After(set);
		EXPECT_NE(
			StreamBufferInitializer::invokeUnderflow(buffer.get()), streambuf::traits_type::eof());
	}

	TEST_F(StreamBufferTest, ReadFailure)
	{
		Fixtures::Lib lib;
		::ZipDirFs::Zip::Base::Lib::File* data =
			reinterpret_cast<decltype(data)>(::Test::rand(UINT32_MAX));
		content->data = data;
		auto bufferPtr = GenerateRandomBufferedContentState();
		StreamBufferInitializer::invokeSetg(buffer.get(), content->buffer,
			content->buffer + content->lastWrite, content->buffer + content->lastWrite);
		EXPECT_CALL(lib, fread(data, content->buffer + content->lastWrite, randomLenBase()))
			.WillOnce(Throw(std::bad_alloc()));
		EXPECT_THROW(StreamBufferInitializer::invokeUnderflow(buffer.get()), std::bad_alloc);
	}
} // namespace Test::ZipDirFs::Zip
