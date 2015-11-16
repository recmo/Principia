#pragma once
#include <DFG/PropertyMap.h>

class Node;
class OrderProperty: public PropertyMap::Property
{
public:
	OrderProperty(const OrderProperty& copy);
	OrderProperty(const std::vector<const Node*>& nodes);
	virtual ~OrderProperty();
	virtual void print(std::wostream& out) const override;
	
	const std::vector<const Node*>& nodes() const { return _nodes; }
	std::vector<const Node*>& nodes() { return _nodes; }
	
protected:
	std::vector<const Node*> _nodes;
};

