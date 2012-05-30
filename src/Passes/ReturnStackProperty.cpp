#include "Passes/ReturnStackProperty.h"

PropertyType ReturnStackProperty::classType = PropertyType::ReturnStack;

ReturnStackProperty::ReturnStackProperty(const ReturnStackProperty& copy)
: Property()
, _positions(copy.positions())
{
}

ReturnStackProperty::ReturnStackProperty(const std::vector<int>& positions)
: Property()
, _positions(positions)
{
}

ReturnStackProperty::~ReturnStackProperty()
{
}

void ReturnStackProperty::print(std::wostream& out) const
{
	out << _positions;
}
