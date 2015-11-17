#include "ClosureProperty.h"
#include <DFG/Edge.h>

ClosureProperty::ClosureProperty(const ClosureProperty& copy)
: _edges(copy._edges)
{
}

ClosureProperty::ClosureProperty(const std::vector<std::shared_ptr<Edge>>& edges)
: _edges(edges)
{
}

ClosureProperty::~ClosureProperty()
{
}

void ClosureProperty::print(std::wostream& out) const
{
	out << _edges;
}

