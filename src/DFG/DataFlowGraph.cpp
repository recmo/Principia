#include "DataFlowGraph.h"

void DataFlowGraph::addNode(std::shared_ptr<Node> node)
{
	_nodes.insert(node);
}
