#include <Command.h>
#include <fstream>
#include <Unicode/convert.h>
#include <Parser/Parser.h>

Command parse(L"parse", [](Command::Arguments arguments) {
	assert(arguments.size() == 1);
	
	// First pass, build AST
	// Second pass, bind all identifiers
	Parser::Node n = Parser::parseFile(arguments[0]);
	
	uint i = 0;
	std::function<void(const Parser::Node& n)> r = [&](const Parser::Node& n) -> void {
		
		for(uint j = 0; j < i; ++j)
			std::wcerr << "\t";
		if(n.kind == Parser::Module)
			std::wcerr << "Module";
		if(n.kind == Parser::Scope)
			std::wcerr << "Scope";
		if(n.kind == Parser::Statement)
			std::wcerr << "Statement";
		if(n.kind == Parser::SubStatement)
			std::wcerr << "SubStatement";
		if(n.kind == Parser::Identifier)
			std::wcerr << "Identifier";
		if(n.kind == Parser::Quote)
			std::wcerr << "Quote";
		std::wcerr << " " << n.quote << n.identifier << n.filename;
		if(n.is_binding_site)
			std::wcerr << " binding";
		if(std::shared_ptr<Parser::Node> site = n.binding_site.lock())
			std::wcerr << " bound to " << site->identifier;
		std::wcerr << "\n";
		
		++i;
		for(const std::shared_ptr<Parser::Node>& c: n.children)
			r(*c);
		--i;
	};
	r(n);
	
	return Command::success;
},
L"parse a source file",
L"<source file>\n\n"
"The source file is parsed and the resulting structure printed to\n"
"the standard output.\n"
);
