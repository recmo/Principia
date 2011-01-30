#pragma once
#include "fixups.h"
#include "Symbol.h"

class Closure;

class ClosureDefinition
{
public:
	Symbol* function;
	vector<Symbol*> arguments;
	vector<Symbol*> returns;
};

std::wostream& operator<<(std::wostream& out, const ClosureDefinition* cd);
