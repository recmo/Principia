#include "Passes/ClosureProperty.h"
#include <DFG/Edge.h>

PropertyType ClosureProperty::classType = PropertyType::Closure;

ClosureProperty::ClosureProperty(const ClosureProperty& copy)
: _edges(copy._edges)
{
}

ClosureProperty::ClosureProperty(const std::vector<const Edge*>& edges)
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

