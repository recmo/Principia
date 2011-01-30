#pragma once
#include "fixups.h"
#include "Value.h"

class Context;
class ClosureDefinition;

class Closure
{
public:
	Closure(const ClosureDefinition* closure, const Context* context);
	vector<Value> evaluate(const vector<Value>& arguments) const;
	
	const ClosureDefinition* const closure;
	const Context* const context;
};

