#ifndef __DEBUGGER_HPP__
#define __DEBUGGER_HPP__

#include <utility>
#include <string>
#include <linux/types.h>
#include <unordered_map>

class breakpoint
{
public:
	breakpoint() = default;
	breakpoint(pid_t process_id, std::intptr_t address)
		: _process_id(process_id)
		, _address(address)
	{}

	std::intptr_t get_address() const { return _address; }
	bool is_enabled() const { return _enabled; }

	void enable();
	void disable();

private:
	pid_t _process_id;
	std::intptr_t _address;
	uint8_t _data;
	bool _enabled;
};

class debugger
{
public:
	debugger(std::string executable_name, pid_t process_id)
		: _executable_name(std::move(executable_name))
		, _process_id(process_id)
	{}

	void run();

private:
	void handle_command(const std::string& command);
	void continue_execution();
	void set_breakpoint(std::intptr_t address);

private:
	std::unordered_map< std::intptr_t, breakpoint > _breakpoints;
	std::string _executable_name;
	pid_t _process_id;
};

#endif
