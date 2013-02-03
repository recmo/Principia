#pragma once
#include <vector>
class DataFlowGraph;
class Node;
class Edge;

/// Anotate the DFG with ConstantProperties for constant calls
class ConstantCall {
public:
	ConstantCall(DataFlowGraph* dfg): _dfg(dfg), _fixedPoint(true) { }
	~ConstantCall() { }
	
	void anotateCalls();
	
	bool fixedPoint() const { return _fixedPoint; }
	
protected:
	DataFlowGraph* _dfg;
	bool _fixedPoint;
};
