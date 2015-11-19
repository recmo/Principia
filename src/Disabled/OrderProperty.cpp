#include "OrderProperty.h"
#include <DFG/Node.h>

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
