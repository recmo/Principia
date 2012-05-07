#pragma once
#include "fixups.h"
#include "DFG/PropertyType.h"
#include "DFG/Property.h"
class Edge;

class ClosureProperty: public Property
{
public:
	ClosureProperty(const ClosureProperty& copy);
	ClosureProperty(const std::vector<const Edge*>& edges);
	virtual ~ClosureProperty();
	
	static PropertyType classType;
	virtual PropertyType type() const { return classType; }
	virtual ClosureProperty* clone() const { return new ClosureProperty(*this); }
	
	virtual void print(std::wostream& out) const;
	
	const std::vector<const Edge*>& edges() const { return _edges; }
	std::vector<const Edge*>& edges() { return _edges; }
	
protected:
	std::vector<const Edge*> _edges;
}; 
