#pragma once
#include "fixups.h"
#include "Value.h"

class BuiltinsStatic: public map<string, BuiltinFunction>
{
public:
	BuiltinsStatic();
};

extern BuiltinsStatic builtins;
