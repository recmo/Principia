#pragma once
class DataFlowGraph;
class Node;

class TopologicalSorter
{
public:
	TopologicalSorter(DataFlowGraph* dfg): _dfg(dfg) { }
	~TopologicalSorter() { }
	
	void sortClosures();
	
private:
	DataFlowGraph* _dfg;
	
	void sortClosure(Node* closure);
};
