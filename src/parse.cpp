#include <Command.h>
#include <fstream>
#include <Unicode/convert.h>
#include <Parser/Parser.h>

Command parse(L"parse", [](Command::Arguments arguments) {
	assert(arguments.size() == 1);
	
	// First pass, build AST
	// Second pass, bind all identifiers
	std::shared_ptr<Parser::Node> n = Parser::parseFile(arguments[0]);
	
	Parser::Program p = Parser::compile(n);
	
	for(const auto s: p.symbols_import)
		std::wcerr << s << " ";
	std::wcerr << "\n";
	for(const auto s: p.symbols_export)
		std::wcerr << s << " ";
	std::wcerr << "\n";
	std::wcerr << p.constants.size() << "\n";
	for(const auto s: p.constants)
		std::wcerr << s.size() << " " << s << "\n";
	for(const auto s: p.closures)
		std::wcerr << s << " ";
	for(const auto s: p.calls) {
		for(const auto q: s)
			std::wcerr << q.first << " " << q.second << " ";
		std::wcerr << "\n";
	}
	
	return Command::success;
},
L"parse a source file",
L"<source file>\n\n"
"The source file is parsed and the resulting structure printed to\n"
"the standard output.\n"
);
