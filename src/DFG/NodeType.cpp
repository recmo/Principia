#include "NodeType.h"
#include <Unicode/exceptions.h>

std::wostream& operator<<(std::wostream& out, const NodeType& nodetype)
{
	switch(nodetype) {
		case NodeType::Call: return out << L"Call";
		case NodeType::Closure: return out << L"Closure";
		default: throw logic_error(L"Invalid enum value.");
	}
}
