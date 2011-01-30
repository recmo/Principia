#pragma once
#include "fixups.h"
#include "Symbol.h"
#include "Value.h"

class ClosureDefinition;

struct Closure
{
	Closure(const ClosureDefinition* closure, const Context& context);
	vector<Value> evaluate(const vector<Value>& arguments) const;
	
	const ClosureDefinition* closure;
	Context context;
};

