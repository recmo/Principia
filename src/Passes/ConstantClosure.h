#pragma once
#include <vector>
#include <memory>
class DataFlowGraph;
class Node;
class Edge;

/// Anotate the DFG with ConstantProperties for empty closures
class ConstantClosure {
public:
	ConstantClosure(DataFlowGraph* dfg): _dfg(dfg), _fixedPoint(true) { }
	~ConstantClosure() { }
	
	void anotateClosures();
	
	bool fixedPoint() const { return _fixedPoint; }
	
protected:
	DataFlowGraph* _dfg;
	bool _fixedPoint;
	bool isRecursiveClosure(std::vector<std::shared_ptr<Node>>& seen, std::shared_ptr<Node> current);
};
