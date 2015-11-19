#pragma once
#include "fixups.h"
#include <DFG/DataFlowGraph.h>

// Find the tree of closures
class ClosureTree
{
public:
	ClosureTree(DataFlowGraph* program);
	~ClosureTree();
	
	void calculate();
	
private:
	DataFlowGraph* _program;
	int _parent;
	vector<int> _parents;
	
	void recurse(Node* node);
};
