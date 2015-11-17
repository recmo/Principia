#pragma once
#include <DFG/PropertyMap.h>
#include <Unicode/string.h>
#include <set>
#include <memory>
class Node;
class Edge;

class DataFlowGraph: public PropertyMap
{
public:
	void addNode(std::shared_ptr<Node> node);
	const std::set<std::shared_ptr<Node>>& nodes() const { return _nodes; }
	
private:
	std::set<std::shared_ptr<Node>> _nodes;
};
