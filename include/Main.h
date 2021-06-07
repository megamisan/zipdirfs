/*
 * Copyright © 2012-2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef MAIN_H
#define MAIN_H

#include <string>
#include <vector>

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
	Main();
	virtual ~Main();
	void Init(const int argc, const char* argv[]);
	void Run();
	inline const std::string getSourcePath() { return this->sourcePath; }

protected:
private:
	std::string sourcePath;
	std::vector<std::string> fuseOptions;
};

extern Main application;

#endif // MAIN_H
