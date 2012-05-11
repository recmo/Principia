#include "Passes/StackProperty.h"

PropertyType StackProperty::classType = PropertyType::Stack;

StackProperty::StackProperty(const StackProperty& copy)
: Property()
, _positions(copy.positions())
{
}

StackProperty::StackProperty(const std::vector<int>& positions)
: Property()
, _positions(positions)
{
}

StackProperty::~StackProperty()
{
}

void StackProperty::print(std::wostream& out) const
{
	out << _positions << endl;
}
