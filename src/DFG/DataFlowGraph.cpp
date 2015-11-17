#include "DataFlowGraph.h"
#include <DFG/Node.h>
#include <DFG/Edge.h>
#include <Parser/IdentifierProperty.h>

void DataFlowGraph::addNode(std::shared_ptr<Node> node)
{
	_nodes.insert(node);
}

std::shared_ptr<Edge> DataFlowGraph::operator[](const string& identifier)
{
	for(auto node: _nodes) {
		for(auto edge: node->out()) {
			if(!edge->has<IdentifierProperty>())
				continue;
			if(edge->get<IdentifierProperty>().value() == identifier)
				return edge;
		}
	}
	return std::shared_ptr<Edge>();
}
