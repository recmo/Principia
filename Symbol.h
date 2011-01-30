#pragma once
#include "fixups.h"
#include "Value.h"

class Symbol;
class ClosureDefinition;
class CallDefinition;

typedef map<Symbol*, Value> Context;

class Symbol
{
public:
	Symbol();
	
	Value evaluate(Context& context);

	string label;
	ClosureDefinition* closure;
	CallDefinition* call;
};

std::wostream& operator<<(std::wostream& out, const Symbol* s);
