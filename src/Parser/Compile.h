#pragma once
#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include <set>
#include <map>
#include <Parser/Parser.h>
namespace Compile {
using Parser::Symbol;

struct Function {
	
	// The functions id. This is the first part of its symbol. The second
	// part is always zero.
	uint id = 0;
	
	// The number of argument the function requires.
	uint arity = 0;
	
	// The name given to the function, if not anonymous.
	std::wstring name;
	
	// Unresolved symbols map to identifier strings.
	std::map<Symbol, std::wstring> imports;
	
	// Constants are always string valued.
	std::map<Symbol, std::wstring> constants;
	
	// Symbols that are closed over. These are used in the function, but not
	// passed as arguments.
	std::vector<Symbol> closure;
	
	// Symbols passed as arguments. Their count should equal the arity.
	std::vector<Symbol> arguments;
	
	// Labels used for the function name and arguments
	std::vector<std::wstring> labels;
	
	// The closures that are allocated in the function body.
	std::vector<uint> allocations;
	
	// The one and only tail call made in the function body.
	std::vector<Symbol> call;
};

typedef std::vector<Function> Program;

std::vector<std::set<uint>> calculate_dependencies(const Parser::Program& p);

std::vector<std::vector<Symbol>> calculate_closures(
	const Parser::Program& p);

std::vector<std::vector<uint>> calculate_allocs(const Parser::Program& p);

Program compile(const Parser::Program& p);

void write(std::wostream& out, const Program& program);

};
