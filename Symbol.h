#pragma once
#include "fixups.h"
#include "Value.h"

class Context;
class ClosureDefinition;
class CallDefinition;

class Symbol
{
public:
	Symbol();
	
	Value evaluate(Context* context);

	string label;
	ClosureDefinition* closure;
	CallDefinition* call;
};

std::wostream& operator<<(std::wostream& out, const Symbol* s);
