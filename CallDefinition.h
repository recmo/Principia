#pragma once
#include "fixups.h"
#include "Symbol.h"

class CallDefinition
{
public:
	void evaluate(Context* context);
	
	Symbol* function;
	vector<Symbol*> arguments;
	vector<Symbol*> returns;
};

std::wostream& operator<<(std::wostream& out, const CallDefinition* cd);

