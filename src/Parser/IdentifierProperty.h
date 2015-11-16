#pragma once
#include <DFG/PropertyMap.h>
#include <Unicode/string.h>

class IdentifierProperty: public PropertyMap::Property
{
public:
	IdentifierProperty(const IdentifierProperty& copy);
	IdentifierProperty(const string& identifier);
	virtual ~IdentifierProperty();
	virtual void print(std::wostream& out) const override;
	
	string value() const { return _identifier; }
	 
protected:
	string _identifier;
};
