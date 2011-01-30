#pragma once
#include "fixups.h"

class Symbol;

class Context
{
public:
	map<Symbol*, uint64> values;
};
