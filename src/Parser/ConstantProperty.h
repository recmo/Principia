#pragma once
#include <DFG/PropertyMap.h>
#include <Passes/Value.h>

class ConstantProperty: public PropertyMap::Property
{
public:
	ConstantProperty(const ConstantProperty& copy);
	ConstantProperty(const Value& value);
	virtual ~ConstantProperty();
	virtual void print(std::wostream& out) const  override;
	
	const Value& value() const { return _value; }
	
protected:
	Value _value;
}; 
