#pragma once
#include <DFG/Property.h>

class Node;
class OrderProperty: public Property
{
public:
	OrderProperty(const OrderProperty& copy);
	OrderProperty(const std::vector<const Node*>& nodes);
	virtual ~OrderProperty();
	
	static PropertyType classType;
	virtual PropertyType type() const { return classType; }
	virtual OrderProperty* clone() const { return new OrderProperty(*this); }
	
	virtual void print(std::wostream& out) const;
	
	const std::vector<const Node*>& nodes() const { return _nodes; }
	std::vector<const Node*>& nodes() { return _nodes; }
	
protected:
	std::vector<const Node*> _nodes;
};

