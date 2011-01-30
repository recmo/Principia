#pragma once
#include "fixups.h"

class Context;
class ClosureDefinition;

class Closure
{
public:
	Closure(const ClosureDefinition* closure, const Context* context);
	vector<uint64> evaluate(const vector<uint64>& arguments) const;
	
	const ClosureDefinition* const closure;
	const Context* const context;
};

