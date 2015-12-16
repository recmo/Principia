#pragma once
#include <DFG/DataFlowGraph.h>
#include <vector>
#include <set>
#include <memory>

/// Annotates the DFG with ClosureProperties
class ClosureCloser {
public:
	explicit ClosureCloser(DataFlowGraph& dfg): _dfg(dfg) { }
	
	void annotateClosures();
	void annotateClosure(Node& closureNode);
	
protected:
	typedef std::set<std::shared_ptr<Node>> NodeSet;
	typedef std::set<std::shared_ptr<OutPort>> OutPortSet;
	const DataFlowGraph & _dfg;
	
	NodeSet closureBody(Node& closure);
	void closureBody(OutPort& out, NodeSet& set);
	void closureBody(InPort& in, NodeSet& set);
	void closureBody(Node& node, NodeSet& set);
};
