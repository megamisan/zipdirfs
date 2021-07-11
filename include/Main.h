/*
 * Copyright © 2012-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef MAIN_H
#define MAIN_H

#include "Options.h"

/**
 * @brief Application entry point
 * @authors Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
class Main
{
public:
	struct Result
	{
		const int result;
		Result(int res) : result(res) {}
		Result(const Result& res) : result(res.result) {}
	};
	Main(const int, const char*[]);
	virtual ~Main();
	void init();
	void run();

protected:
private:
	Options options;
};

extern Main application;

#endif // MAIN_H
