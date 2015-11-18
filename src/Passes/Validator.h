#pragma once
#include <DFG/DataFlowGraph.h>
#include <Utilities/inttypes.h>
#include <vector>
#include <memory>

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
	std::vector<uint> _preorder;
	std::vector<uint> _component;
	std::vector<uint> _unassigned;
	std::vector<uint> _undetermined;
};
