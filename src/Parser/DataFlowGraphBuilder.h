#pragma once
#include <Parser/ParseTree.h>
#include <DFG/DataFlowGraph.h>
#include <map>

// The ParseTree needs to be uninlined and the identifiers bound
class DataFlowGraphBuilder
{
public:
	DataFlowGraphBuilder(ParseTree* parseTree);
	~DataFlowGraphBuilder();
	
	void compile();
	
	DataFlowGraph* dataFlowGraph() { return _dfg; }
	
private:
	ParseTree* _parseTree;
	DataFlowGraph* _dfg;
	
	void declare(ParseTree::Node* node);
	void connect(ParseTree::Node* node);
	
	std::map<ParseTree::Statement*, std::shared_ptr<Node>> _declarations;
	std::map<ParseTree::Identifier*, std::shared_ptr<OutPort>> _identifiers;
};
