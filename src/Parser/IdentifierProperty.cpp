#include "IdentifierProperty.h"

IdentifierProperty::IdentifierProperty(const IdentifierProperty& copy)
: Property(copy)
, _identifier(copy._identifier)
{
}

IdentifierProperty::IdentifierProperty(const string& identifier)
: Property()
, _identifier(identifier)
{
}

IdentifierProperty::~IdentifierProperty()
{
}

void IdentifierProperty::print(std::wostream& out) const
{
	out << _identifier;
}
