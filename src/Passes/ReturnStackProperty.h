#pragma once
#include <DFG/Property.h>

class ReturnStackProperty:  public Property
{
public:
	ReturnStackProperty(const ReturnStackProperty& copy);
	ReturnStackProperty(const std::vector<int>& positions);
	virtual ~ReturnStackProperty();
	
	static PropertyType classType;
	virtual PropertyType type() const { return classType; }
	virtual ReturnStackProperty* clone() const { return new ReturnStackProperty(*this); }
	
	virtual void print(std::wostream& out) const;
	
	const std::vector<int>& positions() const { return _positions; }
	std::vector<int>& positions() { return _positions; }
	
protected:
	std::vector<int> _positions;
};
