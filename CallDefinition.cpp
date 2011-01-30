#include "CallDefinition.h"
#include "Symbol.h"

std::wostream& operator<<(std::wostream& out, const CallDefinition* cd)
{
	if(cd == 0)
	{
		out << L"null";
	}
	else
	{
		out << cd->returns << L" ≔ " << cd->function << cd->arguments;
	}
	return out;
}
