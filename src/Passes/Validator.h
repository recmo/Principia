#pragma once
#include "fixups.h"
#include <DFG/DataFlowGraph.h>

/// @brief Validates correctness of the dfg and adds analysis metadata
class Validator
{
public:
	Validator(DataFlowGraph* program);
	~Validator();
	
	void validate();
	
	int indexOf(Node* node);
	
	void print();
	
	void depthFirstSearch();
	void visit(int i);
	void visit(Node* node);
	
private:
	DataFlowGraph* _program;
	int _preorderCounter;
	int _componentCounter;
	vector<int> _preorder;
	vector<int> _component;
	vector<int> _unassigned;
	vector<int> _undetermined;
};
