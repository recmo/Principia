#include "DFG/EdgeType.h"

string EdgeType::toString() const
{
	switch(value)
	{
		case EdgeType::External: return L"Undefined";
		case EdgeType::Return: return L"Return";
		case EdgeType::Closure: return L"Function";
		case EdgeType::Argument: return L"Argument";
		default: return L"Illegal value";
	}
}

