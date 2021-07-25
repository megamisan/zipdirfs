/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#include "StateReporter.h"
#include "ZipDirFs/Fuse/EntryProxy.h"
#include <bitset>
#include <chrono>
#include <condition_variable>
#include <cwchar>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <set>
#include <sstream>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

typedef std::unique_lock<std::mutex> lock_type;

StateReporter reporter;

namespace std
{
	std::string to_string(const std::thread::id& id)
	{
		std::stringstream ss;
		ss.imbue(std::locale::classic());
		ss << std::hex << id;
		auto res = ss.str();
		return res.substr(res.length() - 6);
	}
} // namespace std

#define CSI "\e["
#define MODE_SET "h"
#define MODE_RESET "l"
#define MODE_REPORT "n"
#define ATTR_ALL "0"
#define ATTR_BOLD "1"
#define ATTR_CURSOR "?25"
#define ATTR_AUTOENDL "?7"
#define REPORT_CPR "6"
#define BOLD CSI ATTR_BOLD MODE_SET
#define UNBOLD CSI ATTR_BOLD MODE_RESET
namespace display
{
	int len(const std::string& src)
	{
		std::mbstate_t s1 = {0};
		const std::string::value_type* bsrc = src.c_str();
		std::size_t len = std::mbsrtowcs(nullptr, &bsrc, 0, &s1);
		if (len != (size_t)-1)
		{
			s1 = {0};
			bsrc = src.c_str();
			std::unique_ptr<std::wstring::value_type[]> dst(new std::wstring::value_type[len + 1]);
			std::mbsrtowcs(dst.get(), &bsrc, len + 1, &s1);
			return wcswidth(dst.get(), len);
		}
		return -1;
	}

	std::string substr(const std::string& source, std::ptrdiff_t start, std::ptrdiff_t length = 0)
	{
		if (start == 0 && length == 0)
		{
			return source;
		}
		std::mbstate_t s1 = {0}, s2;
		const std::string::value_type* bsrc = source.c_str();
		std::size_t len = std::mbsrtowcs(nullptr, &bsrc, 0, &s1);
		if (len != (size_t)-1)
		{
			s1 = {0};
			bsrc = source.c_str();
			std::unique_ptr<std::wstring::value_type[]> dst(new std::wstring::value_type[len + 1]);
			std::mbsrtowcs(dst.get(), &bsrc, len + 1, &s1);
			bsrc = source.c_str();
			decltype(bsrc) esrc = bsrc + source.length(), ref[3]{bsrc, bsrc, bsrc},
						   pos[3]{bsrc, bsrc, bsrc};
			std::size_t refIndexes[3]{0, 0, 0}, posIndexes[3]{0, 0, 0};
			std::ptrdiff_t expected[2]{start, length}, current[3]{0, 0, 0};
			for (std::uint8_t w = 0; w < 2; ++w)
			{
				s1 = s2 = {0};
				while (expected[w] != 0 && dst[posIndexes[w]])
				{
					if (w == 1)
					{
						pos[2] = pos[1];
						posIndexes[2] = posIndexes[1];
						ref[2] = ref[1];
						refIndexes[2] = refIndexes[1];
						current[2] = current[1];
					}
					std::size_t posLen = mbrlen(pos[w], esrc - pos[w], &s2);
					std::size_t posWidth = wcwidth(dst[posIndexes[w]]);
					pos[w] += posLen;
					++posIndexes[w];
					if (expected[w] < 0)
					{
						current[w] -= posWidth;
						while (current[w] < expected[w])
						{
							if (w == 1)
							{
								ref[2] = ref[1];
								refIndexes[2] = refIndexes[1];
								current[2] = current[1];
							}
							std::size_t refLen = mbrlen(ref[w], esrc - ref[w], &s1);
							std::size_t refWidth = wcwidth(dst[refIndexes[w]]);
							ref[w] += refLen;
							++refIndexes[w];
							current[w] += refWidth;
						}
					}
					else
					{
						current[w] += posWidth;
						if (current[w] >= expected[w])
						{
							expected[w] = 0;
						}
					}
				}
				if (w == 0)
				{
					ref[1] = pos[1] = start < 0 ? ref[0] : pos[0];
					refIndexes[1] = posIndexes[1] = start < 0 ? refIndexes[0] : posIndexes[0];
				}
				else if (current[w] > length)
				{
					pos[1] = pos[2];
					ref[1] = ref[2];
					refIndexes[1] = refIndexes[2];
					current[1] = current[2];
				}
			}
			const char *beg = start < 0 ? ref[0] : pos[0],
					   *end = length < 0 ? ref[1] : (length > 0 ? pos[1] : esrc);
			return (end <= beg) ? "" : std::string(beg, end);
		}
		return "";
	}
#define OFFSET_TOP 2
#define OFFSET_MIDDLE 5
#define OFFSET_BOTTOM 8
	const char* const asciiBorders[] = {"|", "-", "+", "+", "+", "+", "+", "+", "+", "+", "+"};
	const char* const utf8Borders[] = {"│", "─", "┌", "┬", "┐", "├", "┼", "┤", "└", "┴", "┘"};
	const char* const* borders = nullptr;
	void start()
	{
		if (borders == nullptr)
		{
#ifdef IUTF8
			struct termios termInfo;
			if (tcgetattr(0, &termInfo) != -1)
			{
				borders = (termInfo.c_iflag & IUTF8) == IUTF8 ? utf8Borders : asciiBorders;
			}
			else
#endif
			{
				std::mbstate_t s;
				const wchar_t* src = L"é";
				borders = (std::wcsrtombs(nullptr, &src, 0, &s) < 2) ? asciiBorders : utf8Borders;
			}
		}
		std::cout << CSI ATTR_ALL MODE_SET CSI ATTR_CURSOR ";" ATTR_AUTOENDL MODE_RESET;
	}
	void end() { std::cout << CSI ATTR_ALL MODE_SET CSI ATTR_CURSOR ";" ATTR_AUTOENDL MODE_SET; }
	int getCurrentColumn()
	{
		int col = -1;
		struct termios termInfo, save;
		bool changed = false;
		if (tcgetattr(0, &termInfo) != -1)
		{
			save = termInfo;
			cfmakeraw(&termInfo);
			if (save.c_iflag != termInfo.c_iflag || save.c_oflag != termInfo.c_oflag
				|| save.c_cflag != termInfo.c_cflag || save.c_lflag != termInfo.c_lflag)
			{
				tcsetattr(0, TCSANOW, &termInfo);
				termInfo = save;
				tcgetattr(0, &termInfo);
				changed = save.c_iflag != termInfo.c_iflag || save.c_oflag != termInfo.c_oflag
					|| save.c_cflag != termInfo.c_cflag || save.c_lflag != termInfo.c_lflag;
			}
		}
		std::cout << CSI REPORT_CPR MODE_REPORT;
		std::cout.flush();
		if (std::cin.peek() == CSI[0] && std::cin.get() == CSI[0] && std::cin.peek() == CSI[1]
			&& std::cin.get() == CSI[1])
		{
			std::string res;
			bool capture = false;
			int c;
			while ((c = std::cin.get()) != 'R')
			{
				if (capture)
				{
					res += (char)c;
				}
				if (c == ';')
				{
					capture = true;
				}
			}
			col = std::atoi(res.c_str());
		}
		if (changed)
		{
			tcsetattr(0, TCSANOW, &save);
		}
		return col;
	}
} // namespace display

namespace
{
#pragma region Reporting entry
	class ReportingEntry : public ::ZipDirFs::Fuse::EntryProxy
	{
	public:
		fusekit::entry* child(const char*);
		int stat(struct stat&);
		int access(int);
		int chmod(mode_t);
		int open(fuse_file_info&);
		int release(fuse_file_info&);
		int read(char*, size_t, off_t, fuse_file_info&);
		int write(const char*, size_t, off_t, fuse_file_info&);
		int opendir(fuse_file_info&);
		int readdir(void*, fuse_fill_dir_t, off_t, fuse_file_info&);
		int releasedir(fuse_file_info&);
		int mknod(const char*, mode_t, dev_t);
		int unlink(const char*);
		int mkdir(const char*, mode_t);
		int rmdir(const char*);
		int flush(fuse_file_info&);
		int truncate(off_t);
		int utimens(const timespec[2]);
		int readlink(char*, size_t);
		int symlink(const char*, const char*);
		int setxattr(const char*, const char*, size_t, int);
		int getxattr(const char*, char*, size_t);
		int listxattr(char*, size_t);
		int removexattr(const char*);
	};

	fusekit::entry* ReportingEntry::child(const char* name)
	{
		reportSegmentPath(name);
		auto res = EntryProxy::child(name);
		if (res == nullptr)
		{
			reportDone();
		}
		return res;
	}

	int ReportingEntry::stat(struct stat& stbuf)
	{
		::reportAction("stat");
		auto res = EntryProxy::stat(stbuf);
		::reportDone();
		return res;
	}

	int ReportingEntry::access(int mode)
	{
		::reportAction("access");
		auto res = EntryProxy::access(mode);
		::reportDone();
		return res;
	}

	int ReportingEntry::chmod(mode_t permission)
	{
		::reportAction("chmod");
		auto res = EntryProxy::chmod(permission);
		::reportDone();
		return res;
	}

	int ReportingEntry::open(fuse_file_info& fi)
	{
		::reportAction("open " + std::to_string(fi.fh));
		auto res = EntryProxy::open(fi);
		::reportDone();
		return res;
	}

	int ReportingEntry::release(fuse_file_info& fi)
	{
		::reportAction("release " + std::to_string(fi.fh));
		auto res = EntryProxy::release(fi);
		::reportDone();
		return res;
	}

	int ReportingEntry::read(char* buf, size_t size, off_t offset, fuse_file_info& fi)
	{
		::reportAction("read " + std::to_string(fi.fh) + ": " + std::to_string(size) + "@"
			+ std::to_string(offset));
		auto res = EntryProxy::read(buf, size, offset, fi);
		::reportDone();
		return res;
	}

	int ReportingEntry::write(const char* buf, size_t size, off_t offset, fuse_file_info& fi)
	{
		::reportAction("write " + std::to_string(fi.fh) + ": " + std::to_string(size) + "@"
			+ std::to_string(offset));
		auto res = EntryProxy::write(buf, size, offset, fi);
		::reportDone();
		return res;
	}

	int ReportingEntry::opendir(fuse_file_info& fi)
	{
		::reportAction("opendir " + std::to_string(fi.fh));
		auto res = EntryProxy::opendir(fi);
		::reportDone();
		return res;
	}

	int ReportingEntry::readdir(void* buf, fuse_fill_dir_t filter, off_t offset, fuse_file_info& fi)
	{
		::reportAction("readdir " + std::to_string(fi.fh));
		auto res = EntryProxy::readdir(buf, filter, offset, fi);
		::reportDone();
		return res;
	}

	int ReportingEntry::releasedir(fuse_file_info& fi)
	{
		::reportAction("releasedir " + std::to_string(fi.fh));
		auto res = EntryProxy::releasedir(fi);
		::reportDone();
		return res;
	}

	int ReportingEntry::mknod(const char* name, mode_t mode, dev_t type)
	{
		::reportAction("mknod " + std::string(name) + " (" + std::to_string(type) + ")");
		auto res = EntryProxy::mknod(name, mode, type);
		::reportDone();
		return res;
	}

	int ReportingEntry::unlink(const char* name)
	{
		::reportAction("unlink " + std::string(name));
		auto res = EntryProxy::unlink(name);
		::reportDone();
		return res;
	}

	int ReportingEntry::mkdir(const char* name, mode_t mode)
	{
		::reportAction("mkdir " + std::string(name));
		auto res = EntryProxy::mkdir(name, mode);
		::reportDone();
		return res;
	}

	int ReportingEntry::rmdir(const char* name)
	{
		::reportAction("rmdir " + std::string(name));
		auto res = EntryProxy::rmdir(name);
		::reportDone();
		return res;
	}

	int ReportingEntry::flush(fuse_file_info& fi)
	{
		::reportAction("flush " + std::to_string(fi.fh));
		auto res = EntryProxy::flush(fi);
		::reportDone();
		return res;
	}

	int ReportingEntry::truncate(off_t off)
	{
		::reportAction("truncate " + std::to_string(off));
		auto res = EntryProxy::truncate(off);
		::reportDone();
		return res;
	}

	int ReportingEntry::utimens(const timespec times[2])
	{
		::reportAction("utimens");
		auto res = EntryProxy::utimens(times);
		::reportDone();
		return res;
	}

	int ReportingEntry::readlink(char* buf, size_t bufsize)
	{
		::reportAction("readlink");
		auto res = EntryProxy::readlink(buf, bufsize);
		::reportDone();
		return res;
	}

	int ReportingEntry::symlink(const char* name, const char* target)
	{
		::reportAction("symlink " + std::string(name) + " → " + std::string(target));
		auto res = EntryProxy::symlink(name, target);
		::reportDone();
		return res;
	}

	int ReportingEntry::setxattr(const char* name, const char* value, size_t size, int flags)
	{
		::reportAction("setxattr " + std::string(name));
		auto res = EntryProxy::setxattr(name, value, size, flags);
		::reportDone();
		return res;
	}

	int ReportingEntry::getxattr(const char* name, char* value, size_t size)
	{
		::reportAction("getxattr " + std::string(name));
		auto res = EntryProxy::getxattr(name, value, size);
		::reportDone();
		return res;
	}

	int ReportingEntry::listxattr(char* list, size_t size)
	{
		::reportAction("listxattr");
		auto res = EntryProxy::listxattr(list, size);
		::reportDone();
		return res;
	}

	int ReportingEntry::removexattr(const char* name)
	{
		::reportAction("removexattr " + std::string(name));
		auto res = EntryProxy::removexattr(name);
		::reportDone();
		return res;
	}
#pragma endregion
#pragma region Internal data and helpers
	struct LockState
	{
		std::thread::id locked;
		std::set<std::thread::id> locking;
		std::set<std::thread::id> waiting;
	};
	struct Archive
	{
		std::string path;
		std::map<void*, std::string> files;
	};
	struct TablesState
	{
		std::vector<std::thread::id> threads;
		std::map<std::string, LockState> locks;
		std::size_t maxActionLength = 6, maxPathLength = 4, maxLockLength = 4, maxLocking = 0,
					maxWaiting = 0, maxArchivePath = 7, maxFilePath = 4;
	};
	std::map<void*, Archive> archives;
	std::map<void*, void*> fileArchive;
	// 0 = Activated, 1 = Actions, 2 = Locks, 3 = Ended, 4 = Files
	std::bitset<5> reportState{0b11110};
	const std::map<std::string, int> reportFlags{
		{"actions", 1}, {"locks", 2}, {"ended", 3}, {"files", 4}};

	inline size_t lenOkOrZero(int len) { return len == -1 ? 0 : (size_t)len; }

	void analyzeRequest(
		const std::thread::id& id, const StateReporter::Request& request, TablesState& state)
	{
		if (id != std::thread::id())
		{
			state.threads.push_back(id);
		}
		state.maxActionLength =
			std::max(state.maxActionLength, lenOkOrZero(display::len(request.action)));
		state.maxPathLength =
			std::max(state.maxPathLength, lenOkOrZero(display::len(request.path)));
		for (auto& lock : request.locks)
		{
			if (lock.second != "free")
			{
				auto& lockState = state.locks[lock.first];
				state.maxLockLength =
					std::max(state.maxLockLength, lenOkOrZero(display::len(lock.first)));
				if (lock.second == "lock")
				{
					lockState.locked = id;
				}
				else if (lock.second == "init")
				{
					lockState.locking.insert(id);
					state.maxLocking = std::max(state.maxLocking, lockState.locking.size());
				}
				else
				{
					lockState.waiting.insert(id);
					state.maxWaiting = std::max(state.maxWaiting, lockState.waiting.size());
				}
			}
		}
	}
	void analyzeArchive(const Archive& archive, TablesState& state)
	{
		state.maxArchivePath =
			std::max(state.maxArchivePath, lenOkOrZero(display::len(archive.path)));
		for (auto& pair : archive.files)
		{
			state.maxFilePath =
				std::max(state.maxFilePath, lenOkOrZero(display::len(pair.second)));
		}
	}

	struct Column
	{
		std::size_t position;
		bool nextLine;
		std::size_t length;
	};

	template <size_t n>
	void arrangeColumns(Column columns[n], int cols)
	{
		const size_t startCol = columns[0].position + columns[0].length;
		size_t lastCol = columns[0].position + columns[0].length + 1;
		for (size_t col = 1; col < n; ++col)
		{
			if (columns[col].position + 1 + columns[col].length > cols)
			{
				lastCol = std::max(lastCol, columns[col].position);
				columns[col].position = startCol;
				columns[col].nextLine = true;
				if (col + 1 < n)
				{
					columns[col + 1].position = startCol + columns[col].length + 1;
				}
			}
		}
		if (columns[n - 1].length > cols - columns[n - 1].position - 1)
		{
			columns[n - 1].length = cols - columns[n - 1].position - 1;
		}
	}
#pragma endregion
#pragma region Write helpers
	void fillPosition(std::size_t expected, std::size_t& position)
	{
		if (position < expected)
		{
			std::size_t blank = expected - position;
			std::cout << std::string(blank, ' ');
			position = expected;
		}
	}
	void fillPosition(std::size_t expected, std::size_t& position, const char* c)
	{
		if (position < expected)
		{
			std::size_t blank = expected - position;
			while (blank > 0)
			{
				std::cout << c;
				--blank;
			}
			position = expected;
		}
	}
	void prepareNextLine(
		const Column& col, std::size_t& position, const char* const end, const char* const start)
	{
		if (col.nextLine)
		{
			std::cout << end << '\n';
			position = 0;
			if (col.position > 1 && start != nullptr)
			{
				std::cout << start;
				position = 1;
			}
		}
	}
	void writeHeaderColumn(const Column& col, std::size_t& position, const std::string& text)
	{
		prepareNextLine(col, position, display::borders[0], display::borders[0]);
		fillPosition(col.position - 1, position);
		std::cout << display::borders[0] << BOLD << text << UNBOLD;
		position += 1 + display::len(text);
		fillPosition(col.position + col.length, position);
	}
	void writeBodyColumn(const Column& col, std::size_t& position, const std::string& text)
	{
		prepareNextLine(col, position, display::borders[0], display::borders[0]);
		fillPosition(col.position - 1, position);
		std::cout << display::borders[0] << text;
		position += 1 + display::len(text);
		fillPosition(col.position + col.length, position);
	}
	void writeBodyColumnThreads(const Column& col, std::size_t& position,
		const std::set<std::thread::id>& list, const std::string& separator)
	{
		prepareNextLine(col, position, display::borders[0], display::borders[0]);
		fillPosition(col.position - 1, position);
		std::cout << display::borders[0];
		position += 1;
		bool isFirst = true;
		const auto separatorLen = display::len(separator);
		for (const auto& item : list)
		{
			if (isFirst)
			{
				isFirst = false;
			}
			else
			{
				std::cout << separator;
				position += separatorLen;
			}
			auto text = std::to_string(item);
			std::cout << text;
			position += display::len(text);
		}
		fillPosition(col.position + col.length, position);
	}
#pragma endregion
#pragma region Display helpers
	void writeLineColumn(const Column& col, std::size_t& position, int offset, bool isStart)
	{
		prepareNextLine(col, position, display::borders[offset + 2],
			offset != OFFSET_BOTTOM ? display::borders[0] : nullptr);
		fillPosition(col.position - 1, position);
		std::cout << display::borders[offset + (isStart ? 0 : 1)];
		++position;
		fillPosition(col.position + col.length, position, display::borders[1]);
	}
	const std::string requestHeaders[] = {"Thread", "Action", "Path"};
	const std::string lockHeaders[] = {"Lock", "Locked", "Locking", "Waiting"};
	const std::string fileHeaders[] = {"Archive", "Ref", "File"};
	template <size_t n>
	void writeLine(const Column columns[n], int offset)
	{
		std::size_t position = 0;
		for (int col = 0; col < n; ++col)
		{
			writeLineColumn(columns[col], position, offset, col == 0 || columns[col].nextLine);
		}
		std::cout << display::borders[offset + 2] << '\n';
	}
	template <size_t n>
	void writeHeader(
		const Column columns[n], const std::string headers[n], bool separatorBefore = false)
	{
		if (separatorBefore)
		{
			writeLine<n>(columns, OFFSET_MIDDLE);
		}
		std::size_t position = 0;
		for (int col = 0; col < n; ++col)
		{
			writeHeaderColumn(columns[col], position, headers[col]);
		}
		std::cout << display::borders[0] << '\n';
		writeLine<n>(columns, OFFSET_MIDDLE);
	}
	void writeRequest(
		const StateReporter::Request& request, const Column columns[3], const std::string& thread)
	{
		std::size_t position = 0;
		std::string temp;
		writeBodyColumn(columns[0], position, thread);
		temp = request.action;
		if (display::len(temp) > columns[1].length)
		{
			temp = display::substr(temp, 0, columns[1].length);
		}
		writeBodyColumn(columns[1], position, temp);
		temp = request.path;
		if (display::len(temp) > columns[2].length)
		{
			temp = "…" + display::substr(temp, -columns[2].length);
			size_t newLen = display::len(temp);
			if (newLen < columns[2].length)
			{
				temp = std::string(columns[2].length - newLen, ' ') + temp;
			}
		}
		writeBodyColumn(columns[2], position, temp);
		std::cout << display::borders[0] << '\n';
	}
	void writeLock(const LockState& state, const Column columns[4], const std::string& lock)
	{
		std::size_t position = 0;
		std::string temp;
		writeBodyColumn(columns[0], position, lock);
		writeBodyColumn(columns[1], position,
			state.locked != std::thread::id() ? std::to_string(state.locked) : "");
		writeBodyColumnThreads(columns[2], position, state.locking, " ");
		writeBodyColumnThreads(columns[3], position, state.waiting, " ");
		std::cout << display::borders[0] << '\n';
	}
#pragma endregion
#pragma region Run Helpers
	struct Cycle
	{
	public:
		Cycle(std::size_t max) : max(max) { cycle = 0; }
		template <class iterator>
		void walk(iterator it, iterator end, std::function<void(iterator)> doWork,
			std::function<void()> doCycle)
		{
			for (; !(it == end); ++it)
			{
				if (cycle == max)
				{
					doCycle();
					cycle = 0;
				}
				doWork(it);
				++cycle;
			}
		}
		void reset() { cycle = 0; }

	private:
		size_t cycle;
		const size_t max;
	};

	class ArchiveFile
	{
		struct Column
		{
			std::string archive;
			std::string ref;
			std::string file;
		};
		class Iterator;

	public:
		typedef const Column value_type;
		typedef value_type& reference;
		typedef value_type* pointer;
		typedef Iterator iterator;
		iterator begin() const { return Iterator(archives.begin(), archives.end()); }
		iterator end() const { return Iterator(archives.end()); }

	private:
		class Iterator : public std::iterator<std::forward_iterator_tag, value_type, size_t>
		{
		public:
			Iterator(const decltype(archives)::iterator end) : itArc(end), endItArc(end){};
			Iterator(
				const decltype(archives)::iterator begin, const decltype(archives)::iterator end) :
				itArc(begin),
				endItArc(end)
			{
				advance(true);
			};
			reference operator*() const { return column; }
			Iterator& operator++()
			{
				advance();
				return *this;
			}
			bool operator==(const Iterator& other) const
			{
				return other.endItArc == endItArc && other.itArc == itArc
					&& other.endItFile == endItFile && other.itFile == itFile;
			}

		private:
			void advance(bool first = false)
			{
				if (itArc != endItArc && !first)
				{
					column.archive.clear();
					if (itFile != endItFile)
					{
						++itFile;
					}
					if (itFile == endItFile)
					{
						endItFile = itFile = decltype(itFile)();
						++itArc;
					}
				}
				if (itArc != endItArc)
				{
					if (itFile == endItFile)
					{
						column.archive = itArc->second.path;
						if (itArc->second.files.size() == 0)
						{
							column.ref = "--";
							column.file = "--";
						}
						else
						{
							endItFile = itArc->second.files.end();
							itFile = itArc->second.files.begin();
						}
					}
					if (itFile != endItFile)
					{
						std::stringstream ss;
						ss.imbue(std::locale::classic());
						ss << itFile->first;
						column.ref = ss.str().substr(2);
						column.file = itFile->second;
					}
				}
			}
			decltype(archives)::iterator itArc;
			decltype(archives)::iterator endItArc;
			decltype(Archive::files)::iterator itFile;
			decltype(Archive::files)::iterator endItFile;
			Column column;
		};
	};

	bool checkReport(Options& options)
	{
		bool isExpected = options.hasOption("report");
		if (isExpected)
		{
			std::string request = options.getOption("report");
			reportState[0] = true;
			if (request != "all" && request != "")
			{
				reportState = 1;
				std::string::size_type start = 0, end;
				while (start != std::string::npos)
				{
					end = request.find(';', start);
					std::string flag = request.substr(
						start, (end == std::string::npos ? request.length() : end) - start);
					auto it = reportFlags.find(flag);
					if (it != reportFlags.end())
					{
						reportState[it->second] = true;
					}
					start = end == std::string::npos ? end : (end + 1);
				}
			}
		}
		options.unsetOption("report");
		return isExpected;
	}

	void analyzeAllRequests(TablesState& tablesState, const StateReporter::requests_type& requests,
		const StateReporter::endedRequests_type& endedRequests)
	{
		for (auto& request : requests)
		{
			analyzeRequest(request.first, request.second, tablesState);
		}
		if (reportState[3])
		{
			for (auto& request : endedRequests)
			{
				analyzeRequest(std::thread::id(), request, tablesState);
			}
		}
	}

	void analyzeAllArchives(TablesState& tablesState)
	{
		for (auto& archive : archives)
		{
			analyzeArchive(archive.second, tablesState);
		}
	}

	void writeRequests(const TablesState& tablesState, const StateReporter::requests_type& requests,
		const StateReporter::endedRequests_type& endedRequests, const Column threadColumns[3],
		size_t rows)
	{
		std::size_t num = 0, maxRows = rows / 2;
		for (size_t i = 0; i < 3; ++i)
		{
			if (threadColumns[i].nextLine)
			{
				maxRows /= 2;
			}
		}
		Cycle cycle(std::max((std::size_t)4, maxRows));
		writeLine<3>(threadColumns, OFFSET_TOP);
		writeHeader<3>(threadColumns, requestHeaders);
		cycle.walk<StateReporter::requests_type::const_iterator>(
			requests.begin(), requests.end(),
			[&threadColumns, &tablesState](StateReporter::requests_type::const_iterator it)
			{ writeRequest(it->second, threadColumns, std::to_string(it->first)); },
			[&threadColumns]() { writeHeader<3>(threadColumns, requestHeaders, true); });
		if (reportState[3])
		{
			cycle.walk<StateReporter::endedRequests_type::const_iterator>(
				endedRequests.begin(), endedRequests.end(),
				[&threadColumns, &num](StateReporter::endedRequests_type::const_iterator it)
				{ writeRequest(*it, threadColumns, "*" + std::to_string(num++)); },
				[&threadColumns]() { writeHeader<3>(threadColumns, requestHeaders, true); });
		}
		writeLine<3>(threadColumns, OFFSET_BOTTOM);
	}

	void writeLocks(const TablesState& tablesState, const Column lockColumns[4], size_t rows)
	{
		std::size_t num = 0, maxRows = rows / 2;
		for (size_t i = 0; i < 4; ++i)
		{
			if (lockColumns[i].nextLine)
			{
				maxRows /= 2;
			}
		}
		Cycle cycle(std::max((std::size_t)4, maxRows));
		writeLine<4>(lockColumns, OFFSET_TOP);
		writeHeader<4>(lockColumns, lockHeaders);
		cycle.walk<decltype(TablesState::locks)::const_iterator>(
			tablesState.locks.begin(), tablesState.locks.end(),
			[&lockColumns, &tablesState](decltype(TablesState::locks)::const_iterator it)
			{ writeLock(it->second, lockColumns, it->first); },
			[&lockColumns]() { writeHeader<4>(lockColumns, lockHeaders, true); });
		writeLine<4>(lockColumns, OFFSET_BOTTOM);
	}

	void writeFiles(const TablesState& tablesState, const Column fileColumns[3], size_t rows)
	{
		std::size_t num = 0, maxRows = rows / 2;
		for (size_t i = 0; i < 3; ++i)
		{
			if (fileColumns[i].nextLine)
			{
				maxRows /= 2;
			}
		}
		Cycle cycle(std::max((std::size_t)4, maxRows));
		writeLine<3>(fileColumns, OFFSET_TOP);
		writeHeader<3>(fileColumns, fileHeaders);
		ArchiveFile af;
		cycle.walk<ArchiveFile::iterator>(
			af.begin(), af.end(),
			[&fileColumns, &tablesState](ArchiveFile::iterator it)
			{
				std::size_t position = 0;
				std::string temp;
				writeBodyColumn(fileColumns[0], position, (*it).archive);
				writeBodyColumn(fileColumns[1], position, (*it).ref);
				temp = (*it).file;
				if (display::len(temp) > fileColumns[2].length)
				{
					temp = "…" + display::substr(temp, -fileColumns[2].length);
					size_t newLen = display::len(temp);
					if (newLen < fileColumns[2].length)
					{
						temp = std::string(fileColumns[2].length - newLen, ' ') + temp;
					}
				}
				writeBodyColumn(fileColumns[2], position, temp);
				std::cout << display::borders[0] << '\n';
			},
			[&fileColumns]() { writeHeader<3>(fileColumns, fileHeaders, true); });
		writeLine<3>(fileColumns, OFFSET_BOTTOM);
	}
#pragma endregion
#pragma region Logging
	std::mutex logWrite, logTime;
	std::set<std::string> categories;

	void parseCategories(const std::string& value)
	{
		std::string::size_type start = 0, end;
		while (start != std::string::npos)
		{
			end = value.find(';', start);
			categories.emplace(std::move(
				value.substr(start, (end == std::string::npos ? value.length() : end) - start)));
			start = end == std::string::npos ? end : (end + 1);
		}
	}

	const std::tm toLocalTime(const std::time_t time)
	{
		lock_type lock(logTime);
		return *std::localtime(&time);
	}

	std::ostream& writeTime(std::ostream& str)
	{
		const auto now{std::chrono::system_clock::now()};
		const auto nanoseconds{
			std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count()
			% 1000000};
		const auto time{std::chrono::system_clock::to_time_t(now)};
		const auto localtime{toLocalTime(time)};
		str << std::put_time(&localtime, "%F %T") << '.' << std::setw(6) << std::setfill('0')
			<< nanoseconds << std::put_time(&localtime, "%z");
		return str;
	}
#pragma endregion
} // namespace
#pragma region StateReporter
StateReporter::StateReporter() : active(false) {}

void StateReporter::start(::ZipDirFs::Fuse::EntryProxy& ep, Options& options)
{
	bool isForeground = options.hasArgument("-f");
	checkReport(options);
	if (isForeground && reportState[0] && isatty(STDIN_FILENO) && isatty(STDOUT_FILENO))
	{
		std::locale::global(std::locale(""));
		std::cin.imbue(std::locale());
		std::cout.imbue(std::locale());
		std::cerr.imbue(std::locale());
		active = true;
		auto reporting = std::unique_ptr<ReportingEntry>(new ReportingEntry());
		*static_cast<::ZipDirFs::Fuse::EntryProxy*>(reporting.get()) = std::move(ep);
		ep = std::unique_ptr<::fusekit::entry>(std::move(reporting));
		worker = decltype(worker)(new std::thread(StateReporter::run));
	}
	if (options.hasOption("log"))
	{
		log = decltype(log)(
			new std::ofstream(options.getOption("log"), std::ios_base::out | std::ios_base::app));
		if (log->fail())
		{
			if (isForeground)
			{
				std::cerr << "Failed to open " << options.getOption("log") << " for writing.\n";
			}
			log = nullptr;
		}
		options.unsetOption("log");
	}
	if (options.hasOption("logcat"))
	{
		parseCategories(options.getOption("logcat"));
		options.unsetOption("logcat");
	}
	if (log != nullptr)
	{
		std::stringstream ss;
		ss.imbue(std::locale::classic());
		ss << '[' << writeTime << "] ==== START";
		*log << ss.str() << std::endl;
	}
}

void StateReporter::stop()
{
	if (active)
	{
		active = false;
		worker->join();
	}
	{
		lock_type lock(logWrite);
		if (log != nullptr)
		{
			std::stringstream ss;
			ss.imbue(std::locale::classic());
			ss << '[' << writeTime << "] ==== STOP";
			*log << ss.str() << std::endl;
			log = nullptr;
		}
	}
}

void StateReporter::run()
{
	// TODO: Implement
	std::condition_variable v;
	lock_type lock(reporter.access);
	std::chrono::seconds delay(2);
	bool sizeError = false;
	display::start();
	while (reporter.active)
	{
		v.wait_for(lock, delay);
		struct winsize size;
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
		if (size.ws_col < 40)
		{
			if (!sizeError)
			{
				std::cerr << "Your display does not have enought columns for proper report."
						  << std::endl;
			}
			sizeError = true;
			continue;
		}
		sizeError = false;
		TablesState tablesState;
		bool reported = false;
		if (reporter.requests.size() + reporter.endedRequests.size() > 0)
		{
			analyzeAllRequests(tablesState, reporter.requests, reporter.endedRequests);
			Column threadColumns[] = {/* ID */ {1, false, 6},
				{8, false, tablesState.maxActionLength},
				{9 + tablesState.maxActionLength, false, tablesState.maxPathLength}};
			Column lockColumns[] = {{1, false, tablesState.maxLockLength},
				{tablesState.maxLockLength + 2, false, 6},
				{tablesState.maxLockLength + 9, false,
					std::max((std::size_t)7,
						tablesState.maxLocking > 1 ? tablesState.maxLocking * 7 - 1 : 0)},
				{tablesState.maxLockLength + 10
						+ std::max((std::size_t)7,
							tablesState.maxLocking > 1 ? tablesState.maxLocking * 7 - 1 : 0),
					false,
					std::max((std::size_t)7,
						tablesState.maxWaiting > 1 ? tablesState.maxWaiting * 7 - 1 : 0)}};
			if (reportState[1])
			{
				reported = true;
				arrangeColumns<3>(threadColumns, size.ws_col);
				writeRequests(tablesState, reporter.requests, reporter.endedRequests, threadColumns,
					size.ws_row);
			}
			if (reportState[2] && tablesState.locks.size() > 0)
			{
				if (reported)
				{
					std::cout << "\n";
				}
				reported = true;
				arrangeColumns<4>(lockColumns, size.ws_col);
				writeLocks(tablesState, lockColumns, size.ws_row);
			}
		}
		if (archives.size() && reportState[4])
		{
			if (reported)
			{
				std::cout << "\n";
			}
			reported = true;
			analyzeAllArchives(tablesState);
			Column fileColumns[] = {{1, false, tablesState.maxArchivePath},
				{tablesState.maxArchivePath + 2, false, sizeof(ArchiveFile::pointer) << 1},
				{tablesState.maxArchivePath + (sizeof(ArchiveFile::pointer) << 1) + 3, false,
					tablesState.maxFilePath}};
			arrangeColumns<3>(fileColumns, size.ws_col);
			writeFiles(tablesState, fileColumns, size.ws_row);
		}
		std::cout << "====\n";
		reporter.endedRequests.clear();
	}
	display::end();
}
#pragma endregion
#pragma region Global helpers
fusekit::entry* reportWrap(fusekit::entry* e)
{
	if (reporter.active)
	{
		auto r = new ReportingEntry();
		*static_cast<ZipDirFs::Fuse::EntryProxy*>(r) = std::unique_ptr<fusekit::entry>(e);
		return r;
	}
	return e;
}

void reportSegmentPath(const std::string& segment)
{
	lock_type lock(reporter.access);
	auto id = std::this_thread::get_id();
	auto reqIt = reporter.requests.find(id);
	if (reqIt == reporter.requests.end())
	{
		reqIt = reporter.requests.emplace(id, StateReporter::Request()).first;
	}
	reqIt->second.path += "/" + segment;
}

void reportAction(const std::string& action)
{
	lock_type lock(reporter.access);
	auto id = std::this_thread::get_id();
	auto reqIt = reporter.requests.find(id);
	if (reqIt == reporter.requests.end())
	{
		reqIt = reporter.requests.emplace(id, StateReporter::Request()).first;
		reqIt->second.path = "/";
	}
	reqIt->second.action = action;
}

void reportDone()
{
	lock_type lock(reporter.access);
	auto id = std::this_thread::get_id();
	auto reqIt = reporter.requests.find(id);
	if (reqIt != reporter.requests.end())
	{
		if (reportState[3])
		{
			reporter.endedRequests.push_back(std::move(reqIt->second));
		}
		reporter.requests.erase(reqIt);
	}
}
std::string buildId(std::string prefix, std::uint64_t value, size_t len)
{
	std::stringstream ss;
	ss.imbue(std::locale::classic());
	ss << std::hex << value;
	auto res = ss.str();
	return prefix + res.substr(res.length() > len ? res.length() - len : 0);
}
void reportArchiveOpened(const std::string& path, void* archive)
{
	lock_type lock(reporter.access);
	archives.emplace(archive, Archive{path, {}});
}
void reportArchiveClosing(void* archive)
{
	lock_type lock(reporter.access);
	archives.erase(archive);
}
void reportFileOpened(void* archive, const std::string& name, void* file)
{
	lock_type lock(reporter.access);
	auto& files = archives[archive].files;
	files.emplace(file, name);
	fileArchive.emplace(file, archive);
}
void reportFileClosing(void* file)
{
	lock_type lock(reporter.access);
	archives[fileArchive[file]].files.erase(file);
	fileArchive.erase(file);
}
#pragma endregion
#pragma region Lock
StateReporter::Lock::Lock(const std::string& name) : name(name)
{
	lock_type lock(reporter.access);
	auto id = std::this_thread::get_id();
	auto reqIt = reporter.requests.find(id);
	if (reqIt == reporter.requests.end())
	{
		reqIt = reporter.requests.emplace(id, StateReporter::Request()).first;
	}
	reqIt->second.locks[name] = "free";
}

StateReporter::Lock::~Lock()
{
	unset();
}

void StateReporter::Lock::init()
{
	setState("init");
}

void StateReporter::Lock::set()
{
	setState("lock");
}

void StateReporter::Lock::unset()
{
	setState("free");
}

void StateReporter::Lock::condStart()
{
	setState("wait");
}

void StateReporter::Lock::setState(const char* state)
{
	lock_type lock(reporter.access);
	auto id = std::this_thread::get_id();
	auto reqIt = reporter.requests.find(id);
	if (reqIt != reporter.requests.end())
	{
		reqIt->second.locks[name] = state;
	}
}
#pragma endregion
#pragma region Log
StateReporter::Log::Log(const std::string& category) :
	valid(!categories.size() || categories.find(category) != categories.end())
{
	stream.imbue(std::locale::classic());
	stream << '[' << writeTime << "] [" << category << "] ["
		   << std::to_string(std::this_thread::get_id()) << "] ";
}
StateReporter::Log::~Log()
{
	if (reporter.log != nullptr && valid)
	{
		lock_type lock(logWrite);
		if (reporter.log != nullptr)
		{
			*reporter.log << stream.str() << std::endl;
		}
	}
}
#pragma endregion
