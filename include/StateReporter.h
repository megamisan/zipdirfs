/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef STATEREPORTER_H
#define STATEREPORTER_H

#include "Options.h"
#include "ZipDirFs/Fuse/EntryProxy.h"
#include <map>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

fusekit::entry* reportWrap(fusekit::entry*);
void reportSegmentPath(const std::string&);
void reportAction(const std::string&);
void reportDone();
std::string buildId(std::string prefix, std::uint64_t value, size_t len);
void reportArchiveOpened(const std::string&, void*);
void reportArchiveClosing(void*);
void reportFileOpened(void*, const std::string&, void*);
void reportFileClosing(void*);

class StateReporter
{
public:
	struct Request;
	typedef std::map<std::thread::id, Request> requests_type;
	typedef std::vector<Request> endedRequests_type;
	StateReporter();
	void start(::ZipDirFs::Fuse::EntryProxy&, Options&);
	void stop();

	struct Lock
	{
		Lock(const std::string&);
		~Lock();
		void init();
		void set();
		void unset();
		void condStart();

	private:
		std::string name;
		void setState(const char*);
	};

	struct Request
	{
		std::string path;
		std::string action;
		std::map<std::string, std::string> locks;
	};

	struct Log
	{
		Log(const std::string& category);
		~Log();
		std::stringstream stream;

	private:
		bool valid;
	};

private:
	static void run();

	bool active;
	std::mutex access;
	std::unique_ptr<std::thread> worker;
	std::unique_ptr<std::ostream> log;
	requests_type requests;
	endedRequests_type endedRequests;
	friend class StateReporter::Lock;
	friend class StateReporter::Log;
	friend fusekit::entry* reportWrap(fusekit::entry*);
	friend void reportSegmentPath(const std::string&);
	friend void reportAction(const std::string&);
	friend void reportDone();
	friend void reportArchiveOpened(const std::string&, void*);
	friend void reportArchiveClosing(void*);
	friend void reportFileOpened(void*, const std::string&, void*);
	friend void reportFileClosing(void*);
};

extern StateReporter reporter;

template <class T>
T condEnd(StateReporter::Lock& l, T&& v)
{
	l.set();
	return v;
}

#endif
