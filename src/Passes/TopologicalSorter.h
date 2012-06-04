#pragma once
#include <vector>
#include <Passes/StackVMProperty.h>

class Node;
class Edge;
class DataFlowGraph;
class Node;

class TopologicalSorter
{
public:
	TopologicalSorter(DataFlowGraph* dfg): _dfg(dfg), _closure(0), _stack(), _order() { }
	~TopologicalSorter() { }
	
	void sortClosures();
	
private:
	DataFlowGraph* _dfg;
	Node* _closure;
	std::vector<const Edge*> _stack;
	std::vector<StackVMProperty::Instruction*> _order;
	
	void sortClosure();
	void sortClosure(const Node* node);
	
};
