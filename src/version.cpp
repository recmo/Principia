#include <Command.h>

const std::wstring name = NAME;
const std::wstring version = VERSION;

Command cmd_version(
	L"version",
	[](Command::Arguments arguments) {
		std::wcout << name << " version " << version << "\n";
		return Command::success;
	},
	L"show version"
);
