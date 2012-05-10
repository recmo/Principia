#pragma once
#include "fixups.h"
#include "DFG/DataFlowGraph.h"
#include "Interpreter/Value.h"

class ClosureNode;
class Closure;

class Interpreter
{
public:
	static void evaluateFunction(const Node* closureNode, const Value* closure, int cl, const Value* arguments, int al, Value* returns, int rl);
	
	Interpreter() { }
	~Interpreter() { }
	
	Value evaluateEdge(const Edge* symbol);
	
private:
	typedef std::map<const Edge*, Value> Context;
	Context _context;
	
	vector<Value> evaluateFunction(const Edge* function, const vector<Value>& arguments);
	Closure* makeClosure(const Node* closure);
	void evaluateFunction(const Node* function);
};
