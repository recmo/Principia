#include "IR/CallNode.h"
#include "IR/SymbolVertex.h"

std::wostream& operator<<(std::wostream& out, const CallNode* cd)
{
	if(cd == 0)
	{
		out << L"null";
	}
	else
	{
		out << cd->returns() << L" ≔ " << cd->function() << cd->arguments();
	}
	return out;
}
