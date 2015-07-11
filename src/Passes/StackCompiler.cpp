#include <Passes/StackCompiler.h>
#include "ClosureProperty.h"
#include "OrderProperty.h"
#include <DFG/DataFlowGraph.h>
#include <DFG/Node.h>
#include <Parser/ConstantProperty.h>

#define debug false

void StackCompiler::sortClosures()
{
	for(Node* node: _dfg->nodes()) {
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

void StackCompiler::sortClosure()
{
	if(debug)
		wcerr << endl << L"Compiling closure " << _closure << endl;
	
	// Initialize the stack with the closure and the arguments
	for(const Edge* edge: _closure->get<ClosureProperty>().edges())
		_stack.push_back(edge);
	for(uint i = 1; i < _closure->outArity(); ++i)
		_stack.push_back(_closure->out(i));
	
	// Start from the returns
	StackMachineProperty::ReturnInstruction* returnInst = new StackMachineProperty::ReturnInstruction(_closure);
	for(const Edge* edge: _closure->in()) {
		if(edge->has<ConstantProperty>()) {
			returnInst->addReturnValue(-1);
			continue;
		}
		if(!contains(_stack, edge))
			sortClosureNode(edge->source());
		returnInst->addReturnValue(indexOf(_stack, edge));
	}
	
	// Add the return instruction
	_order.push_back(returnInst);
	
	// Add OrderProperty
	if(debug) {
		wcerr << _closure << " stack " << _stack << endl;
		wcerr << _closure << " order " << _order << endl;
	}
	StackMachineProperty svmp(_stack, _order);
	if(debug)
		wcerr << svmp << endl;
	_closure->set(svmp);
}

void StackCompiler::sortClosureNode(Node* node)
{
	if(debug)
		wcerr << L"Compiling node " << node << endl;
	
	int closureIndex = -1;
	vector<const Edge*> sources;
	StackMachineProperty::CallInstruction* call;
	if(node->type() == NodeType::Closure) {
		// Add the closure node before its dependencies are evaluated
		if(debug)
			wcerr << "Alloc " << node << endl;
		_order.push_back(new StackMachineProperty::AllocateInstruction(node));
		closureIndex = _stack.size();
		_stack.push_back(node->out(0));
		
		// Find the nodes sources
		sources = node->get<ClosureProperty>().edges();
	} else {
		// Allocate a call instruction
		if(debug)
			wcerr << "Call " << node << endl;
		call = new StackMachineProperty::CallInstruction(node);
		call->numReturns(node->outArity());
		
		// Find the nodes sources
		sources = node->constIn();
	}
	if(debug)
		wcerr << node << " sources " << sources << endl;
	
	// Recurse on the sources
	int i = 0;
	for(const Edge* edge: sources) {
		int valueIndex = -1;
		if(!edge->has<ConstantProperty>()) {
			if(!contains(_stack, edge))
				sortClosureNode(edge->source());
			valueIndex = indexOf(_stack, edge);
		}
		if(node->type() == NodeType::Closure) {
			if(debug)
				wcerr << "Store " << node << " " << i << " " << edge << endl;
			_order.push_back(new StackMachineProperty::StoreInstruction(closureIndex, i, valueIndex));
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
		for(const Edge* edge: node->out())
			_stack.push_back(edge);
	}
}

