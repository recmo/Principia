#pragma once
#include "fixups.h"
#include "DFG/PropertyType.h"
#include "DFG/Property.h"

class Identifier: public Property
{
public:
	Identifier(const Identifier& copy);
	Identifier(const string& identifier);
	virtual ~Identifier();
	
	static PropertyType classType;
	virtual PropertyType type() const { return classType; }
	virtual Identifier* clone() const { return new Identifier(*this); }
	
	virtual void print(std::wostream& out) const;
	
	string value() const { return _identifier; }
	 
protected:
	string _identifier;
};
