#include <sys/wait.h>
#include <sys/ptrace.h>
#include <iostream>
#include <vector>
#include <string>

#include "debugger.hpp"
#include "linenoise/linenoise.h"

namespace {
	std::vector<std::string> split(
		const std::string& value,
		const std::string& delimiters
	)
	{
		std::vector<std::string> items;
		size_t startPos = 0;

		while(true) 
		{
			size_t foundPos = value.find_first_of(delimiters, startPos);
			if (foundPos == std::string::npos)
			{
				if (startPos < value.size())
					items.push_back(value.substr(startPos));

				break;
			}
			
			if (startPos - foundPos >= 1)
				items.push_back(value.substr(startPos, foundPos - startPos));
			
			startPos = foundPos + 1;
		}

		return items;
	}
}

void
breakpoint::enable()
{
	auto data = ptrace(PTRACE_PEEKDATA, _process_id, _address, nullptr);
	std::cout << "Data peeked: " << std::hex << data << std::endl;
	_data = static_cast<uint8_t>(data & 0xff); //save bottom byte
	uint64_t int3 = 0xcc;
	uint64_t data_with_int3 = ((data & ~0xff) | int3); //set bottom byte to 0xcc
	ptrace(PTRACE_POKEDATA, _process_id, _address, data_with_int3);
	std::cout << "Data poked:" << std::hex << data_with_int3 << std::endl;

	_enabled = true;
}

void
breakpoint::disable()
{
	auto data = ptrace(PTRACE_PEEKDATA, _process_id, _address, nullptr);
    auto restored_data = ((data & ~0xff) | _data);
    ptrace(PTRACE_POKEDATA, _process_id, _address, restored_data);

    _enabled = false;
}

void
debugger::run()
{
	int wait_status;
	int options = 0;
	waitpid(_process_id, &wait_status, options);

	char* line = nullptr;
	while((line = linenoise("minidbg> ")))
	{
		handle_command(line);
		linenoiseHistoryAdd(line);
		linenoiseFree(line);
	}
}

void
debugger::handle_command(const std::string& command)
{
	auto args = split(command, " ");
	if (args.empty())
		return;

	auto& first_command = args[0];
	if (first_command == "cont")
	{
		continue_execution();
	}
	else if (first_command == "break")
	{
		std::string address{args[1], 2};
		set_breakpoint(std::stol(address, 0, 16));
	}
	else
	{
		std::cerr << "Unknown command" << std::endl;
	}
}

void
debugger::continue_execution()
{
	ptrace(PTRACE_CONT, _process_id, nullptr, nullptr);
	std::cout << "Continue execution" << std::endl;

    int wait_status;
    int options = 0;
    waitpid(_process_id, &wait_status, options);
}

void
debugger::set_breakpoint(std::intptr_t address)
{
	breakpoint bp{_process_id, address};
	bp.enable();
	std::cout << "Breakpoint set to 0x" << std::hex << address << std::endl;
	_breakpoints[address] = bp;
}