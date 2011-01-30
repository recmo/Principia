#pragma once
#include "fixups.h"
#include "Value.h"

class Symbol;

class Context
{
public:
	map<Symbol*, Value> values;
};
