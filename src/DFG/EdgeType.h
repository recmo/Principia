#pragma once
#include "fixups.h"

class EdgeType
{
public:
	EdgeType(int v = 0) : value(v) { }
	operator int() const { return value; }
	string toString() const;
	bool isValid() const { return value >= 0 && value <= 3; }
	
	const static int External = 0;
	const static int Return = 1;
	const static int Closure = 2;
	const static int Argument = 3;
	
private:
	int value;
};
