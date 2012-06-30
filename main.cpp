#include <fusekit/deamon.h>
#include <vector>

int main(int argc, char *argv[])
{
	std::vector<char*> arguments(argv, argv+argv);
	arguments.push_back(const_cast<char *>("-o ro,nosuid,noexec,noatime,nodiratime"));
	return 0;
}
