#pragma once
#include <DFG/DataFlowGraph.h>

class Verifier
{
public:
	Verifier(const DataFlowGraph* dfg): _dfg(dfg) { }
	~Verifier() { }
	
	void verify();
	
private:
	typedef std::map<std::shared_ptr<Edge>, std::shared_ptr<Edge>> Connections;
	const DataFlowGraph *_dfg;
	std::vector<std::pair<std::shared_ptr<Edge>, Connections> > _propositions;
	
	void verify(std::shared_ptr<Node> closure);
	void assume(std::shared_ptr<Edge> statement,  const Connections& connections = Connections());
	void verify(std::shared_ptr<Edge> statement,  const Connections& connections = Connections());
};

