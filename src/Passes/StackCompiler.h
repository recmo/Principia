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
	std::shared_ptr<Node> _closure;
	std::vector<std::shared_ptr<Edge>> _stack;
	std::vector<StackMachineProperty::Instruction*> _order;
	
	void sortClosure();
	void sortClosureNode(std::shared_ptr<Node> node);
};
