#include "DFG/NodeType.h"

string NodeType::toString() const
{
	switch(value)
	{
		case NodeType::Call: return L"Call";
		case NodeType::Closure: return L"Closure";
		default: return L"Invalid value";
	}
}

