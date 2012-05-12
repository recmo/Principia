#include "StackMachine.h"
#include "Closure.h"
#include "Builtins.h"
#include "DFG/DataFlowGraph.h"
#include <Parser/ConstantProperty.h>
#include "Passes/ClosureProperty.h"
#include <Passes/OrderProperty.h>
#include <Passes/StackProperty.h>
#include <Passes/ReturnStackProperty.h>

#define debug false

vector<Value> StackMachine::evaluateFunction(const Node* closureNode, const vector<Value>& closure, const vector<Value>& arguments)
{
	if(debug)
		wcerr << closureNode << " " << closure << " " << arguments << endl;
	assert(closureNode);
	assert(closureNode->type() == NodeType::Closure);
	assert(closureNode->outArrity() - 1 == arguments.size());
	assert(closureNode->get<ClosureProperty>().edges().size() == 0);
	
	StackMachine sm;
	sm._stack << closure << arguments;
	sm.evaluateCall(closureNode);
	return sm._stack;
}

void StackMachine::evaluateCall(const Node* closure)
{
	if(debug)
		wcerr << "Eval  " << closure << endl;
	assert(closure->has<ClosureProperty>());
	assert(closure->has<OrderProperty>());
	assert(closure->has<ReturnStackProperty>());
	
	// node is a closure nodes
	// stack is […, <closure>, <arguments>]
	// transform it to […, <returns>]
	
	// Determine the base of the stack
	int base = _stack.size();
	base -= closure->get<ClosureProperty>().edges().size();
	base -= closure->outArrity() - 1;
	
	// Process the functions body
	foreach(const Node* node, closure->get<OrderProperty>().nodes()) {
		vector<int> stackPositions = node->get<StackProperty>().positions();
		
		if(node->type() == NodeType::Call) {
			Value closureVal;
			if(stackPositions[0] == -1)
				closureVal = node->in(0)->get<ConstantProperty>().value();
			else
				closureVal = _stack[base + stackPositions[0]].closure();
			
			if(closureVal.kind == Value::Builtin) {
				
				// Call the builtin function
				vector<Value> args;
				for(int i = 1; i < stackPositions.size(); ++i) {
					if(stackPositions[i] == -1)
						args << node->in(i)->get<ConstantProperty>().value();
					else
						args << _stack[base + stackPositions[i]];
				}
				if(debug)
					wcerr << "Builtin " << closureVal << " " << args << endl;
				_stack << closureVal.builtin()(args);
				
			} else {
				
				// Push the closure
				_stack << closureVal.closure()->context();
				
				// Push the arguments
				for(int i = 1; i < stackPositions.size(); ++i) {
					if(stackPositions[i] == -1)
						_stack << node->in(i)->get<ConstantProperty>().value();
					else
						_stack << _stack[base + stackPositions[i]];
				}
				
				// Evaluate the call
				if(debug)
					wcerr << "Call " << closureVal.closure()->node()<< endl;
				evaluateCall(closureVal.closure()->node());
			}
			
		
		} else {
			for(int i = 0; i < stackPositions.size(); ++i) {
				if(stackPositions[i] == -1) {
					// Constant value
					_stack << node->get<ClosureProperty>().edges()[i]->get<ConstantProperty>().value();
				} else {
					_stack << _stack[base + stackPositions[i]];
				}
			}
			if(debug)
				wcerr << "Closure " << node << endl;
			evaluateClosure(node);
		}
	}
	
	// Extract the return values
	int returnBase = _stack.size();
	const vector<int>& returnStackIndices = closure->get<ReturnStackProperty>().positions();
	
	// Push the returns on the stack
	for(int i = 0; i < closure->inArrity(); ++i) {
		if(returnStackIndices[i] == -1)
			_stack << closure->in(i)->get<ConstantProperty>().value();
		else
			_stack << _stack[base + returnStackIndices[i]];
	}
	
	// Copy them to the front of the stack
	for(int i = 0; i < closure->inArrity(); ++i)
		_stack[base + i] = _stack[returnBase + i];
	
	// Erase the rest of the stack
	_stack.erase(_stack.begin() + base + closure->inArrity(), _stack.end());
}


void StackMachine::evaluateClosure(const Node* node)
{
	// node is a closure nodes
	// stack is […, <closure>]
	// transform it to […, Closure]
	
	int numClosureSize = node->get<ClosureProperty>().edges().size();
	vector<Value> closureValues;
	closureValues.insert(closureValues.end(), _stack.end() - numClosureSize, _stack.end());
	_stack.erase(_stack.end() - numClosureSize, _stack.end());
	Closure* closure = new Closure(node, closureValues);
	_stack << Value(closure);
}
