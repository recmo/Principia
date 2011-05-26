#pragma once
#include "fixups.h"
#include "DFG/PropertyMap.h"

class Node;

class Edge: public PropertyMap
{
public:
	Edge(Node* source = 0);
	~Edge();
	
	Node* source() const { return _source; }
	const vector<Node*>& sinks() const { return _sinks; }
	
	void addSink(Node* node);
	void delSink(const Node* node);
	
	void replaceWith(Edge* edge);
	
private:
	Edge(const Edge& copy);
	void operator=(const Edge& copy);
	
	Node* _source;
	vector<Node*> _sinks;
	friend class Node;
};

