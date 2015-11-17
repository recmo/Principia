#include "DataFlowGraph.h"
#include <DFG/Node.h>
#include <DFG/Edge.h>
#include <Parser/IdentifierProperty.h>

void DataFlowGraph::addNode(std::shared_ptr<Node> node)
{
	_nodes.insert(node);
}
