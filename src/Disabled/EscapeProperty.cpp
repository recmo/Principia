#include "EscapeProperty.h"

void EscapeProperty::print(std::wostream& out) const
{
	out << ((_escapes) ? L"may escape" : L"does not escape");
}
