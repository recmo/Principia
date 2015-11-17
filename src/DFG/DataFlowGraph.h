#pragma once
#include <DFG/PropertyMap.h>
#include <DFG/Node.h>
#include <set>
#include <memory>

class DataFlowGraph: public PropertyMap
{
public:
	void addNode(std::shared_ptr<Node> node);
	const std::set<std::shared_ptr<Node>>& nodes() const { return _nodes; }
	
private:
	std::set<std::shared_ptr<Node>> _nodes;
};
