#pragma once
#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <Parser/Parser.h>
namespace Compile {
using Parser::Symbol;

struct Function {
	uint id = 0;
	uint arity = 0;
	std::wstring name;
	std::map<Symbol, std::wstring> imports;
	std::map<Symbol, std::wstring> constants;
	std::vector<Symbol> closure;
	std::vector<Symbol> arguments;
	std::vector<uint> allocations;
	std::vector<Symbol> call;
};

typedef std::vector<Function> Program;

std::vector<std::set<uint>> calculate_dependencies(const Parser::Program& p);

std::vector<uint> calculate_closure_calls(const Parser::Program& p);

std::vector<std::vector<Symbol>> calculate_closures(
	const Parser::Program& p);

std::vector<std::vector<uint>> calculate_allocs(const Parser::Program& p);

Program compile(const Parser::Program& p);

void write(std::wostream& out, const Program& program);

};
