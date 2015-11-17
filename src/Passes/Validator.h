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
	
	int indexOf(std::shared_ptr<Node> node);
	
	void print();
	
	void depthFirstSearch();
	void visit(int i);
	void visit(std::shared_ptr<Node> node);
	
private:
	DataFlowGraph* _program;
	uint _preorderCounter;
	uint _componentCounter;
	vector<uint> _preorder;
	vector<uint> _component;
	vector<uint> _unassigned;
	vector<uint> _undetermined;
};
