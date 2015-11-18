#pragma once
#include <DFG/DataFlowGraph.h>
#include <Utilities/inttypes.h>
#include <vector>
#include <memory>

	// To validate
	// - Take the DFG
	// - Remove first out of closures
	// - Contract Edges and Calls
	// - Verify that the resulting graph is a tree (closure tree?)
	// - For each leaf:
	//   - Take the strongly connected component containing the closure
	//   - Verify that all cycles go through the closure
	//   - Contract the strongly connected component to  


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
