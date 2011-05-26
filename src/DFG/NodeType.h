#pragma once
#include "fixups.h"

class NodeType
{
public:
	NodeType(int v = 0) : value(v) { }
	operator int() const { return value; }
	string toString() const;
	bool isValid() const { return value >= 0 && value <= 1; }
	
	const static int Call = 0;
	const static int Closure = 1;
	
private:
	int value;
};

inline std::wostream& operator<<(std::wostream& out, const NodeType& nodetype)
{
	out << nodetype.toString();
	return out;
}
