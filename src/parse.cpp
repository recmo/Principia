#include <Command.h>
#include <fstream>
#include <Unicode/convert.h>
#include <Parser/Parser.h>

Command parse(L"parse", [](Command::Arguments arguments) {
	assert(arguments.size() == 1);
	
	Parser().parseFile(arguments[0]);
	
	
	return Command::success;
},
L"parse a source file",
L"<source file>\n\n"
"The source file is parsed and the resulting structure printed to\n"
"the standard output.\n"
);
