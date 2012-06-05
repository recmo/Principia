#pragma once
#include <DFG/Property.h>

class EscapeProperty: public Property
{
public:
	EscapeProperty(bool escapes): Property(), _escapes(escapes) { }
	EscapeProperty(const EscapeProperty& copy): Property(), _escapes(copy._escapes) { }
	virtual ~EscapeProperty() { }
	virtual EscapeProperty* clone() const { return new EscapeProperty(*this); }
	static PropertyType classType;
	virtual PropertyType type() const { return classType; }
	virtual void print(std::wostream& out) const;
	
	bool escapes() const { return _escapes; }
	
private:
	bool _escapes;
};
