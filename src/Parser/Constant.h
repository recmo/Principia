#pragma once
#include "fixups.h"
#include "DFG/PropertyType.h"
#include "DFG/Property.h"
#include "Interpreter/Value.h"

class Constant: public Property
{
public:
	Constant(const Constant& copy);
	Constant(const Value& value);
	virtual ~Constant();
	
	static PropertyType classType;
	virtual PropertyType type() const { return classType; }
	virtual Constant* clone() const { return new Constant(*this); }
	
	virtual void print(std::wostream& out) const;
	
	const Value& value() const { return _value; }
	
protected:
	Value _value;
}; 
