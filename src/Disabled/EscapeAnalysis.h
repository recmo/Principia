#pragma once
#include <DFG/DataFlowGraph.h>

class EscapeAnalysis
{
public:
	EscapeAnalysis(DataFlowGraph* dfg): _dfg(dfg) { }
	~EscapeAnalysis() { }
	
	void analyse();
	
private:
	DataFlowGraph *_dfg;
	
	void analyse(std::shared_ptr<Node> closure);
};

