#include "ClosureDefinition.h"
#include "Closure.h"
#include "Symbol.h"

std::wostream& operator<<(std::wostream& out, const ClosureDefinition* cd)
{
	if(cd == 0)
	{
		out << L"null";
	}
	else
	{
		out << cd->function << cd->arguments << L" ↦ " << cd->returns;
	}
	return out;
}
