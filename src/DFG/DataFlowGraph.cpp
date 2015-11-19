#include "DataFlowGraph.h"

Node& DataFlowGraph::make_node(Node::Type type, uint out, uint in)
{
	auto node = std::make_shared<Node>(type, out, in);
	addNode(node);
	return *node;
}

void DataFlowGraph::addNode(std::shared_ptr<Node> node)
{
	_nodes.insert(node);
}
