#pragma once
#include "fixups.h"
#include "DFG/PropertyType.h"
#include "DFG/Property.h"

class IdentifierProperty: public Property
{
public:
	IdentifierProperty(const IdentifierProperty& copy);
	IdentifierProperty(const string& identifier);
	virtual ~IdentifierProperty();
	
	static PropertyType classType;
	virtual PropertyType type() const { return classType; }
	virtual IdentifierProperty* clone() const { return new IdentifierProperty(*this); }
	
	virtual void print(std::wostream& out) const;
	
	string value() const { return _identifier; }
	 
protected:
	string _identifier;
};
