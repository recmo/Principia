#pragma once
#include "fixups.h"

class Context;
class ClosureDefinition;
class CallDefinition;

class Symbol
{
public:
	Symbol();
	
	uint64 evaluate(Context* context);

	string label;
	ClosureDefinition* closure;
	CallDefinition* call;
};

std::wostream& operator<<(std::wostream& out, const Symbol* s);
