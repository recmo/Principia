#pragma once
#include <DFG/PropertyMap.h>
class Node;

class Port: public PropertyMap
{
public:
	Node& parent() { return _parent; }
	const Node& parent() const { return _parent; }
	uint index() const { return _index; }
	
protected:
	Port(Node& parent, uint index)
	: _parent(parent), _index(index) { }
	
private:
	Node& _parent;
	const uint _index;
};
