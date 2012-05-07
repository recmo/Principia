#pragma once
#include <vector>
class DataFlowGraph;
class Node;
class Edge;

/// Anotate the DFG with ClosureProperties
class LambdaLifter {
public:
	LambdaLifter(DataFlowGraph* dfg): _dfg(dfg) { }
	~LambdaLifter() { }
	
	void anotateClosures();
	
protected:
	DataFlowGraph* _dfg;
	
	void anotateClosure(Node* closureNode);
	void recurseOut(Edge* edge, std::vector<Edge*>* edges);
	void recurseOut(Node* node, std::vector<Edge*>* edges);
	void recurseOut(Node* node, std::vector<Node*>* nodes);
	void recurseIn(Edge* edge, std::vector<Edge*>* edges);
	void recurseIn(Node* node, std::vector<Edge*>* edges);
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
