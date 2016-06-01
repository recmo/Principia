#include <Command.h>
#include <Parser/Parser.h>
#include <Parser/Compile.h>
#include <Parser/Interpreter.h>

Command interpret(L"interpret", [](Command::Arguments arguments) {
	assert(arguments.size() == 2);
	
	Parser::Program pp = Parser::compile(Parser::parseFile(arguments[0]));
	Parser::write(std::wcerr, pp);
	Compile::Program cp = Compile::compile(pp);
	Compile::write(std::wcerr, cp);
	std::wcerr << "\n";
	Interpreter::run(cp, arguments[1]);
	
	return Command::success;
},
L"run interpreter",
L"<source file> <funcion>\n\n"
"The source file is parsed and the resulting structure printed to\n"
"the standard output.\n"
);
