#pragma once
#include "fixups.h"

class Symbol;
class CallDefinition;
class ClosureDefinition;

class Program
{
public:
	map<string, Symbol*> symbols;
};

