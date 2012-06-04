#include <Passes/TopologicalSorter.h>
#include "ClosureProperty.h"
#include "OrderProperty.h"
#include <DFG/DataFlowGraph.h>
#include <DFG/Node.h>
#include <Parser/ConstantProperty.h>

#define debug true

/// TODO: Separate closure allocation and closure completion into two different phases

/// TODO: Store instruction: STORE edge IN closureEdge

/// Instructions:
/// CALL(closure, inputs, outputs)
/// ALLOC(closure)
/// STORE(closure, index, input)

void TopologicalSorter::sortClosures()
{
	foreach(Node* node, _dfg->nodes()) {
		if(node->type() != NodeType::Closure)
			continue;
		
		// Set the state
		_closure = node;
		_stack.clear();
		_order.clear();
		
		// Calculate the topological order 
		sortClosure();
	}
}

void TopologicalSorter::sortClosure()
{
	wcerr << endl << L"Sorting " << _closure << endl;
	
	// Initialize the stack with the closure and the arguments
	foreach(const Edge* edge, _closure->get<ClosureProperty>().edges())
		_stack.push_back(edge);
	for(int i = 1; i < _closure->outArrity(); ++i)
		_stack.push_back(_closure->out(i));
	
	// Start from the returns
	StackVMProperty::ReturnInstruction* returnInst = new StackVMProperty::ReturnInstruction;
	foreach(const Edge* edge, _closure->in()) {
		if(edge->has<ConstantProperty>()) {
			returnInst->addReturnValue(-1);
			continue;
		}
		if(!contains(_stack, edge))
			sortClosure(edge->source());
		returnInst->addReturnValue(indexOf(_stack, edge));
	}
	
	// Add the return instruction
	_order.push_back(returnInst);
	
	// Add OrderProperty
	if(debug) {
		wcerr << _closure << " stack " << _stack << endl;
		wcerr << _closure << " order " << _order << endl;
	}
	StackVMProperty svmp(_stack, _order);
	wcerr << svmp << endl;
	_closure->set(svmp);
}

void TopologicalSorter::sortClosure(const Node* node)
{
	int closureIndex = -1;
	vector<const Edge*> sources;
	StackVMProperty::CallInstruction* call;
	if(node->type() == NodeType::Closure) {
		// Add the closure node before its dependencies are evaluated
		if(debug)
			wcerr << "Alloc " << node << endl;
		_order.push_back(new StackVMProperty::AllocateInstruction(node));
		closureIndex = _stack.size();
		_stack.push_back(node->out(0));
		
		// Find the nodes sources
		sources = node->get<ClosureProperty>().edges();
	} else {
		// Allocate a call instruction
		call = new StackVMProperty::CallInstruction(node);
		call->numReturns(node->outArrity());
		
		// Find the nodes sources
		sources = node->in();
	}
	if(debug)
		wcerr << node << " sources " << sources << endl;
	
	// Recurse on the sources
	int i = 0;
	foreach(const Edge* edge, sources) {
		int valueIndex = -1;
		if(!edge->has<ConstantProperty>()) {
			if(!contains(_stack, edge))
				sortClosure(edge->source());
			valueIndex = indexOf(_stack, edge);
		}
		if(node->type() == NodeType::Closure) {
			if(debug)
				wcerr << "Store " << node << " " << i << " " << edge << endl;
			_order.push_back(new StackVMProperty::StoreInstruction(closureIndex, i, valueIndex));
		} else {
			call->addArgument(valueIndex);
		}
		++i;
	}
	
	// Add the call node after al its dependencies are evaluated
	if(node->type() == NodeType::Call) {
		if(debug)
			wcerr << "Call " << node << endl;
		_order.push_back(call);
		
		// Pass all the returns on the stack
		foreach(const Edge* edge, node->out())
			_stack.push_back(edge);
	}
}

