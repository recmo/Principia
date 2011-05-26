#include "Parser/Identifier.h"

PropertyType Identifier::classType = PropertyType::Identifier;

Identifier::Identifier(const Identifier& copy)
: Property(copy)
, _identifier(copy._identifier)
{
}

Identifier::Identifier(const string& identifier)
: Property()
, _identifier(identifier)
{
}

Identifier::~Identifier()
{
}

void Identifier::print(std::wostream& out) const
{
	out << _identifier;
}
