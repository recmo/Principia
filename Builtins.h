#pragma once
#include "fixups.h"

typedef vector<uint64>(*builtinFunction)(const vector<uint64>&);

class BuiltinsStatic: public map<string, builtinFunction>
{
public:
	BuiltinsStatic();
};

extern BuiltinsStatic builtins;
