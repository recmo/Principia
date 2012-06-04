#pragma once
#include <vector>
#include <Passes/StackMachineProperty.h>

class Node;
class Edge;
class DataFlowGraph;
class Node;

class StackCompiler
{
public:
	StackCompiler(DataFlowGraph* dfg): _dfg(dfg), _closure(0), _stack(), _order() { }
	~StackCompiler() { }
	
	void sortClosures();
	
private:
	DataFlowGraph* _dfg;
	Node* _closure;
	std::vector<const Edge*> _stack;
	std::vector<StackMachineProperty::Instruction*> _order;
	
	void sortClosure();
	void sortClosure(const Node* node);
	
};
