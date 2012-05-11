#pragma once
#include <DFG/Property.h>

class StackProperty:  public Property
{
public:
	StackProperty(const StackProperty& copy);
	StackProperty(const std::vector<int>& positions);
	virtual ~StackProperty();
	
	static PropertyType classType;
	virtual PropertyType type() const { return classType; }
	virtual StackProperty* clone() const { return new StackProperty(*this); }
	
	virtual void print(std::wostream& out) const;
	
	const std::vector<int>& positions() const { return _positions; }
	std::vector<int>& positions() { return _positions; }
	
protected:
	std::vector<int> _positions;
};
