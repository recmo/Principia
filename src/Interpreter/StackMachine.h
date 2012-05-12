#pragma once
#include "fixups.h"
#include "DFG/DataFlowGraph.h"
#include "Interpreter/Value.h"

class ClosureNode;
class Closure;

class StackMachine
{
public:
	static vector<Value> evaluateFunction(const Node* closureNode, const vector<Value>& closure, const vector<Value>& arguments);
	
	StackMachine(): _stack() { }
	~StackMachine() { }
	
private:
	vector<Value> _stack;
	
	void evaluateCall(const Node* node);
	void evaluateClosure(const Node* node);
};
