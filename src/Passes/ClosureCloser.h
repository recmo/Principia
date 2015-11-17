#pragma once
#include <vector>
#include <memory>
class DataFlowGraph;
class Node;
class Edge;

/// Annotates the DFG with ClosureProperties
class ClosureCloser {
public:
	ClosureCloser(DataFlowGraph& dfg): _dfg(dfg), _fixedPoint(true) { }
	~ClosureCloser() { }
	
	void anotateClosures();
	
	bool fixedPoint() const { return _fixedPoint; }
	
protected:
	DataFlowGraph const& _dfg;
	bool _fixedPoint;
	
	void anotateClosure(std::shared_ptr<Node> closureNode);
	void recurseOut(std::shared_ptr<Edge> edge, std::vector<std::shared_ptr<Edge>>* edges);
	void recurseOut(std::shared_ptr<Node> node, std::vector<std::shared_ptr<Edge>>* edges);
	void recurseOut(std::shared_ptr<Node> node, std::vector<std::shared_ptr<Node>>* nodes);
	void recurseIn(std::shared_ptr<Edge> edge, std::vector<std::shared_ptr<Edge>>* edges);
	void recurseIn(std::shared_ptr<Node> node, std::vector<std::shared_ptr<Edge>>* edges);
};

// internal nodes are call or closure nodes
// that are required for the return values of the closure
// and depend on the inputs to the closure. This forces
// them to be computed every time the closure is evaluated,
// If the node is a closure node it's internal and lazy
// nodes are not included, even if the results depend on a
// call to the resulting closure.

// lazy nodes are call or closure nodes that are required
// for the return values of this closure and only for the
// return values of this closure without depending on the
// input values. That is, they need to be evaluated exactly
// once before the closure can be evaluated.
