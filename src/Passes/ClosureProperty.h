#pragma once
#include <DFG/PropertyMap.h>
#include <vector>
class Edge;

class ClosureProperty: public PropertyMap::Property
{
public:
	ClosureProperty(const ClosureProperty& copy);
	ClosureProperty(const std::vector<const Edge*>& edges);
	virtual ~ClosureProperty();
	virtual void print(std::wostream& out) const override;
	
	const std::vector<const Edge*>& edges() const { return _edges; }
	std::vector<const Edge*>& edges() { return _edges; }
	
protected:
	std::vector<const Edge*> _edges;
}; 
