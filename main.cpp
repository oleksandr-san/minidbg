#include <iostream>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <unistd.h>
#include <vector>
#include <string>

#include "debugger.hpp"

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		std::cerr << "Progrma name not specified" << std::endl;
		return -1;
	}

	auto exec_path = argv[1];
	auto pid = fork();
	if ( pid == 0 )
	{
		if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0)
		{
			std::cerr << "Error in ptrace\n";
			return -1;
		}

		execl(exec_path, exec_path, nullptr);
	}
	else if (pid >= 1)
	{
		debugger dbg{exec_path, pid};
		dbg.run();
	}

	return 0;
}