#include <Command.h>
#include <Parser/Parser.h>

Command parse(L"parse", [](Command::Arguments arguments) {
	assert(arguments.size() == 1);

	std::shared_ptr<Parser::Node> n = Parser::parseFile(arguments[0]);
	Parser::Program p = Parser::compile(n);
	write(std::wcout, p);
	
	return Command::success;
},
L"parse a source file",
L"<source file>\n\n"
"The source file is parsed and the resulting structure printed to\n"
"the standard output.\n"
);
