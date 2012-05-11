#include <Passes/TopologicalSorter.h>
#include "ClosureProperty.h"
#include "OrderProperty.h"
#include <DFG/DataFlowGraph.h>
#include <DFG/Node.h>
#include <Parser/ConstantProperty.h>

#define debug false

void TopologicalSorter::sortClosures()
{
	foreach(Node* node, _dfg->nodes()) {
		if(node->type() != NodeType::Closure)
			continue;
		
		// Set the state
		_closure = node;
		_closed.clear();
		_arguments.clear();
		_order.clear();
		
		// Calculate the topological order 
		sortClosure();
	}
}

void TopologicalSorter::sortClosure()
{
	assert(_closure->has<ClosureProperty>());
	_closed = _closure->get<ClosureProperty>().edges();
	for(int i = 1; i < _closure->outArrity(); ++i)
		_arguments.push_back(_closure->out(i));
	
	// Start from the returns
	foreach(const Edge* edge, _closure->in()) {
		if(edge->has<ConstantProperty>())
			continue;
		if(contains(_closed, edge))
			continue;
		if(contains(_arguments, edge))
			continue;
		sortClosure(edge->source());
	}
	
	// Add OrderProperty
	_closure->set(OrderProperty(_order));
	if(debug)
		wcerr << _closure << " order " << _order << endl;
}

void TopologicalSorter::sortClosure(const Node* node)
{
	if(contains(_order, node))
		return;
	
	// Find the nodes sources
	vector<const Edge*> sources;
	if(node->type() == NodeType::Call)
		sources = node->in();
	else
		sources = node->get<ClosureProperty>().edges();
	if(debug)
		wcerr << node << " sources " << sources << endl;
	
	// Recurse on the sources
	foreach(const Edge* edge, sources) {
		if(edge->has<ConstantProperty>())
			continue;
		if(contains(_closed, edge))
			continue;
		if(contains(_arguments, edge))
			continue;
		sortClosure(edge->source());
	}
	
	// Add the node after al its dependencies are evaluated
	if(debug)
		wcerr << "Adding " << node << endl;
	_order.push_back(node);
}


