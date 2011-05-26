#include "Parser/Constant.h"

PropertyType Constant::classType = PropertyType::Constant;

Constant::Constant(const Value& value)
: Property()
, _value(value)
{
}

Constant::Constant(const Constant& copy)
: Property(copy)
, _value(copy._value)
{
}

Constant::~Constant()
{
}

void Constant::print(std::wostream& out) const
{
	out << _value;
}



