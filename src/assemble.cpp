#include <Command.h>
#include <Parser/Parser.h>
#include <Parser/Compile.h>
#include <Machine/Machine.h>

Command assemble(L"assemble", [](Command::Arguments arguments) {
	
	assert(arguments.size() == 1);
	
	Parser::Program pp = Parser::compile(Parser::parseFile(arguments[0]));
	// Parser::write(std::wcerr, pp);
	
	Compile::Program cp = Compile::compile(pp);
	//Compile::write(std::wcerr, cp);
	std::wcerr << "\n";
	
	Machine::load(cp);
	// Machine::print();
	Machine::assemble();
	Machine::unload();
	
	return Command::success;
},
L"routput assembler (nasm compatible)",
L"<source file>\n\n"
"The source file is parsed and the resulting structure printed to\n"
"the standard output.\n"
);
