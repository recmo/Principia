#pragma once
#include <vector>

class Node;
class Edge;
class DataFlowGraph;
class Node;

class TopologicalSorter
{
public:
	TopologicalSorter(DataFlowGraph* dfg): _dfg(dfg), _closure(0), _closed(), _arguments(), _order() { }
	~TopologicalSorter() { }
	
	void sortClosures();
	
private:
	DataFlowGraph* _dfg;
	Node* _closure;
	std::vector<const Edge*> _closed;
	std::vector<const Edge*> _arguments;
	std::vector<const Node*> _order;
	
	void sortClosure();
	void sortClosure(const Node* node);
	
};
