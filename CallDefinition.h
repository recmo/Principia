#pragma once
#include "fixups.h"

class Context;
class Symbol;

class CallDefinition
{
public:
	void evaluate(Context* context);
	
	Symbol* function;
	vector<Symbol*> arguments;
	vector<Symbol*> returns;
};

std::wostream& operator<<(std::wostream& out, const CallDefinition* cd);

