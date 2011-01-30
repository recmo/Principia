#pragma once
#include "fixups.h"
#include "Value.h"

typedef vector<Value>(*builtinFunction)(const vector<Value>&);

class BuiltinsStatic: public map<string, builtinFunction>
{
public:
	BuiltinsStatic();
};

extern BuiltinsStatic builtins;
