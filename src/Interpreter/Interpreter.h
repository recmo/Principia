#pragma once
#include "fixups.h"
#include "DFG/DataFlowGraph.h"
#include "Interpreter/Value.h"

class ClosureNode;
class Closure;

class Interpreter
{
public:
	static vector<Value> evaluateFunction(const Node* closureNode, const vector<Value>& closure, const vector<Value>& arguments);
	
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
