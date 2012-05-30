#include "Passes/OrderProperty.h"
#include <DFG/Node.h>

PropertyType OrderProperty::classType = PropertyType::Order;

OrderProperty::OrderProperty(const OrderProperty& copy)
: _nodes(copy._nodes)
{
}

OrderProperty::OrderProperty(const std::vector<const Node*>& nodes)
: _nodes(nodes)
{
}

OrderProperty::~OrderProperty()
{
}

void OrderProperty::print(std::wostream& out) const
{
	out << _nodes;
}
