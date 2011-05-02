#include "IR/ClosureNode.h"
#include "IR/SymbolVertex.h"

ClosureNode::ClosureNode()
{
}

ClosureNode::~ClosureNode()
{
}

std::wostream& operator<<(std::wostream& out, const ClosureNode* cd)
{
	if(cd == 0)
	{
		out << L"null";
	}
	else
	{
		out << cd->function() << cd->arguments() << L" ↦ " << cd->returns();
	}
	return out;
}
