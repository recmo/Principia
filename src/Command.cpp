#include <Command.h>
const std::wstring name = NAME;
const std::wstring version = VERSION;

std::int32_t Main(const std::vector<std::wstring>& arguments)
{
	return Command::main(arguments);
}

std::unique_ptr<std::vector<Command>> Command::_commands;

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
	// TODO: Atomicity
	if(_commands == nullptr) {
		_commands = std::unique_ptr<std::vector<Command>>(
			new std::vector<Command>());
	}
	
	_commands->push_back(*this);
}

std::int32_t Command::main(const std::vector<std::wstring>& arguments)
{
	using namespace std;
	
	// Print help overview
	if(arguments.size() < 2
		|| (arguments.size() == 2 && arguments[1] == L"help")) {
		wcerr << "Usage: " << arguments[0] << " <command> [arguments...]\n"
		<< "where <command> is one of:\n\n";
		for(const Command& command: *_commands) {
			wcerr << "\t" << command._name
			<< "\t\t" << command._description << "\n";
		}
		wcerr << "\n(hint: run \"" << arguments[0] << " help <command>\" "
		<< "to learn more about a command)\n";
		return EXIT_SUCCESS;
	}
	
	// Print detailed help
	if(arguments[1] == L"help") {
		for(const Command& command: *_commands) {
			if(command._name == arguments[2]) {
				wcerr << "Usage: " << arguments[0] << " " << arguments[2] << " "
				<< command._help;
				return EXIT_SUCCESS;
			}
		}
		
		// Unknown command
		wcerr << "Unknown command: " << arguments[2] << "\n"
		<< "(hint: run \"" << arguments[0] << " help\" to see available commands)\n";
		return EXIT_FAILURE;
	}
	
	// Run commands
	for(const Command& command: *_commands) {
		if(command._name == arguments[1]) {
			std::vector<std::wstring> slice;
			std::copy(arguments.begin() + 2, arguments.end(),
				std::back_inserter(slice));
			return command._main(slice);
		}
	}
	
	// Unknown command
	wcerr << name << " version " << version << "\n"
	<< "Unknown command: " << arguments[1] << "\n"
	<< "(hint: run \"" << arguments[0] << " help\" to see available commands)\n";
	return EXIT_FAILURE;
}
