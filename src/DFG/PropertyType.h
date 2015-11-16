#pragma once
#include "fixups.h"

class PropertyType
{
public:
	PropertyType(int v = 0) : value(v) { }
	operator int() const { return value; }
	
	const static int Identifier = 0;
	const static int Source = 1;
	const static int Constant = 2;
	const static int Closure = 3;
	const static int Order = 4;
	const static int Stack = 5;
	const static int ReturnStack = 6;
	const static int Native = 7;
	const static int StackVM = 8;
	const static int Escape = 9;
	const static int Proposition = 10;
	
private:
	int value;
};

std::wostream& operator<<(std::wostream& out, const PropertyType& propertyType);
