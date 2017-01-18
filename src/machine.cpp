#include <Command.h>
#include <Parser/Parser.h>
#include <Parser/Compile.h>
#include <Machine/Machine.h>

Command machine(L"machine", [](Command::Arguments arguments) {
	
	assert(arguments.size() == 1);
	
	Parser::Program pp = Parser::compile(Parser::parseFile(arguments[0]));
	// Parser::write(std::wcerr, pp);
	
	Compile::Program cp = Compile::compile(pp);
	//Compile::write(std::wcerr, cp);
	std::wcerr << "\n";
	
	Machine::load(cp);
	//Machine::print();
	Machine::run();
	Machine::unload();
	
	return Command::success;
},
L"run virtual machine",
L"<source file> <funcion>\n\n"
"The source file is parsed and the resulting structure printed to\n"
"the standard output.\n"
);
