#include "DFG/Edge.h"
#include "Node.h"

Edge::Edge(Node* source)
: PropertyMap()
, _source(source)
, _sinks()
{
}

Edge::~Edge()
{
	for(auto it = _sinks.begin(); it != _sinks.end(); ++it)
		(*it)->forgetEdge(this);
}

void Edge::addSink(Node* node)
{
	for(auto it = _sinks.begin(); it != _sinks.end(); ++it)
		if(*it == node)
			return;
	_sinks.push_back(node);
}

void Edge::delSink(const Node* node)
{
	assert(node != 0);
	auto it = _sinks.begin();
	for(; it != _sinks.end(); ++it)
	{
		Node* n = *it;
		assert(n != 0);
		wcerr << n << endl;
		wcerr << node << endl;
		if(n != node)
			continue;
	}
	_sinks.erase(it);
}

void Edge::replaceWith(Edge* edge)
{
	for(auto it = _sinks.begin(); it != _sinks.end(); ++it)
		(*it)->replaceEdge(this, edge);
	_sinks.empty();
}

