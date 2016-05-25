#include <Command.h>
const std::wstring name = NAME;
const std::wstring version = VERSION;

std::int32_t Main(const std::vector<std::wstring>& arguments)
{
	return Command::main(arguments);
}

std::vector<Command> Command::_commands;

Command::Command(
	const std::wstring& name,
	std::function<std::int32_t(const std::vector<std::wstring>&)> main,
	const std::wstring& description,
	const std::wstring& help)
: _name(name)
, _main(main)
, _description(description)
, _help(help)
{
	_commands.push_back(*this);
}

std::int32_t Command::main(const std::vector<std::wstring>& arguments)
{
	using namespace std;
	wcerr << name << " version " << version << endl;
	
	if(arguments.size() < 1) {
		wcerr << "Usage: proglang source_file [function [arguments]*]" << endl;
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
	return EXIT_FAILURE;
}
