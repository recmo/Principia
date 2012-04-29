#pragma once
#include "fixups.h"
#include "DFG/PropertyType.h"
#include "DFG/Property.h"
#include "Interpreter/Value.h"

class ConstantProperty: public Property
{
public:
	ConstantProperty(const ConstantProperty& copy);
	ConstantProperty(const Value& value);
	virtual ~ConstantProperty();
	
	static PropertyType classType;
	virtual PropertyType type() const { return classType; }
	virtual ConstantProperty* clone() const { return new ConstantProperty(*this); }
	
	virtual void print(std::wostream& out) const;
	
	const Value& value() const { return _value; }
	
protected:
	Value _value;
}; 
