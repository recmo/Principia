#pragma once
#include "fixups.h"
#include "DFG/PropertyMap.h"

class Node;

class Edge: public PropertyMap
{
public:
	Edge(Node* source);
	~Edge();
	
	Node* source() const { return _source; }
	uint sourceIndex() const;
	const vector<Node*>& sinks() const { return _sinks; }
	
	void addSink(Node* node);
	void delSink(const Node* node);
	
	void print(std::wostream& out) const;
	
	void replaceWith(Edge* edge);
	
	bool isFunction() const;
	
private:
	Edge(const Edge& copy);
	void operator=(const Edge& copy);
	
	Node* _source;
	vector<Node*> _sinks;
};

inline std::wostream& operator<<(std::wostream& out, const Edge& edge)
{
	edge.print(out);
	return out;
}

inline std::wostream& operator<<(std::wostream& out, const Edge* edge)
{
	return (edge) ? out << *edge: out << L"nullEdge";
}
