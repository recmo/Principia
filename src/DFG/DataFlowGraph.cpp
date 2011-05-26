#include "DFG/DataFlowGraph.h"
#include "DFG/Node.h"

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
	Node* call = addCall(closure->outArrity() - 1, closure->inArrity());
	call->connect(0, closure->out(0));
	return call;
}
