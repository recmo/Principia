#include "Parser/IdentifierProperty.h"

PropertyType IdentifierProperty::classType = PropertyType::Identifier;

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
