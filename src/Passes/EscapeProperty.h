#pragma once
#include <DFG/PropertyMap.h>

class EscapeProperty: public PropertyMap::Property
{
public:
	EscapeProperty(bool escapes): Property(), _escapes(escapes) { }
	EscapeProperty(const EscapeProperty& copy)
	: Property(), _escapes(copy._escapes) { }
	virtual ~EscapeProperty() { }
	virtual void print(std::wostream& out) const override;
	
	bool escapes() const { return _escapes; }
	
private:
	bool _escapes;
};
