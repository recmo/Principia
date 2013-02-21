#pragma once
#include <DFG/DataFlowGraph.h>

class Verifier
{
public:
	Verifier(const DataFlowGraph* dfg): _dfg(dfg) { }
	~Verifier() { }
	
	void verify();
	
private:
	typedef std::map<const Edge*, const Edge*> Connections;
	const DataFlowGraph *_dfg;
	std::vector<std::pair<const Edge*, Connections> > _assumptions;
	
	void verify(const Node* closure);
	void assume(const Edge* statement,  const Connections& connections = Connections());
	void verify(const Edge* statement,  const Connections& connections = Connections());
};

