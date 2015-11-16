#include "ConstantProperty.h"

ConstantProperty::ConstantProperty(const Value& value)
: Property()
, _value(value)
{
}

ConstantProperty::ConstantProperty(const ConstantProperty& copy)
: Property(copy)
, _value(copy._value)
{
}

ConstantProperty::~ConstantProperty()
{
}

void ConstantProperty::print(std::wostream& out) const
{
	out << _value;
}



