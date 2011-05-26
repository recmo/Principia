#pragma once
#include "fixups.h"
#include "DFG/PropertyType.h"

class Property
{
public:
	virtual ~Property() { }
	
	static PropertyType classType;
	virtual PropertyType type() const = 0;
	virtual Property* clone() const = 0;
	
	virtual void print(std::wostream& out) const = 0;
	
protected:
	Property() { }
};

inline std::wostream& operator<<(std::wostream& out, const Property& prop)
{
	out << prop.type() << L":\t";
	prop.print(out);
	return out;
}
