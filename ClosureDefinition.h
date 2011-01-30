#pragma once
#include "fixups.h"

class Symbol;
class Closure;
class Context;

class ClosureDefinition
{
public:
	Closure* close(Context* context) const;
	
	Symbol* function;
	vector<Symbol*> arguments;
	vector<Symbol*> returns;
};

std::wostream& operator<<(std::wostream& out, const ClosureDefinition* cd);
