#pragma once
#include "fixups.h"

class PropertyType
{
public:
	PropertyType(int v = 0) : value(v) { }
	operator int() const { return value; }
	string toString() const;
	
	const static int Identifier = 0;
	const static int Source = 1;
	const static int Constant = 2;
	const static int Closure = 3;
	const static int Order = 4;
	const static int Stack = 5;
	const static int ReturnStack = 6;
	const static int Native = 7;
	
private:
	int value;
};

inline std::wostream& operator<<(std::wostream& out, const PropertyType& propertyType)
{
	out << propertyType.toString();
	return out;
}
