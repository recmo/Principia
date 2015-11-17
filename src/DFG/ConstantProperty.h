#pragma once
#include <DFG/PropertyMap.h>
#include <DFG/Value.h>

class ConstantProperty: public PropertyMap::Property
{
public:
	ConstantProperty(const ConstantProperty& copy) = default;
	ConstantProperty(const Value& value);
	virtual void print(std::wostream& out) const override;
	
	const Value& value() const { return _value; }
	
protected:
	Value _value;
}; 
