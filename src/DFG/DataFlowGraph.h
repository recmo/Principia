#pragma once
#include <DFG/PropertyMap.h>
#include <DFG/Node.h>
#include <set>
#include <memory>
#include <Utilities/not_null.h>

class DataFlowGraph: public PropertyMap
{
public:

	not_null<std::shared_ptr<Node>> make_node();
	
	void addNode(std::shared_ptr<Node> node);
	const std::set<std::shared_ptr<Node>>& nodes() const { return _nodes; }
	
private:
	std::set<std::shared_ptr<Node>> _nodes;
};
