#include "DFG/DataFlowGraph.h"
#include "DFG/Node.h"
#include <Parser/IdentifierProperty.h>

DataFlowGraph::DataFlowGraph()
: PropertyMap()
, _nodes()
{
}

DataFlowGraph::~DataFlowGraph()
{
	for(auto it = _nodes.begin(); it != _nodes.end(); ++it)
		delete *it;
}

Node* DataFlowGraph::addClosure(int numArgs, int numRets)
{
	Node* closure = new Node(NodeType::Closure, numRets + 1, numArgs);
	_nodes.push_back(closure);
	return closure;
}

Node* DataFlowGraph::addCall(int numArgs, int numRets)
{
	Node* call = new Node(NodeType::Call, numArgs + 1, numRets);
	_nodes.push_back(call);
	return call;
}

Node* DataFlowGraph::addCall(Node* closure)
{
	assert(closure->type() == NodeType::Closure);
	Node* call = addCall(closure->outArity() - 1, closure->inArity());
	call->connect(0, closure->out(0));
	return call;
}

void DataFlowGraph::check()
{
	for(Node* node: _nodes) 
		node->check();
}

Edge* DataFlowGraph::edgeByIdentifier(const string& identifier)
{
	for(Node* node: _nodes) {
		for(Edge* edge: node->out()) {
			if(!edge->has<IdentifierProperty>())
				continue;
			if(edge->get<IdentifierProperty>().value() == identifier)
				return edge;
		}
	}
	return nullptr;
}
