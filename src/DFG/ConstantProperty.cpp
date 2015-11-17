#include "ConstantProperty.h"

ConstantProperty::ConstantProperty(const Value& value)
: Property()
, _value(value)
{
}

void ConstantProperty::print(std::wostream& out) const
{
	out << _value;
}
