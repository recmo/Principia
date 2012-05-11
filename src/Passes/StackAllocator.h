#include "DFG/DataFlowGraph.h"

class StackAllocator
{
public:
	StackAllocator(DataFlowGraph* dfg): _dfg(dfg) { }
	~StackAllocator() { }
	
	void annotate();
	
private:
	DataFlowGraph* _dfg;
};
