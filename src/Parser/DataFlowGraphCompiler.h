#pragma once
#include <map>
#include "ParseTree.h"
class DataFlowGraph;
class Node;
class Edge;

class DataFlowGraphCompiler
{
public:
	DataFlowGraphCompiler(ParseTree* parseTree);
	~DataFlowGraphCompiler();

	void compile();
	
	DataFlowGraph* dataFlowGraph() { return _dfg; }
	
private:
	ParseTree* _parseTree;
	DataFlowGraph* _dfg;
	void declare(ParseTree::Scope* scope);
	void declare(ParseTree::Statement* statement);
	void connect(ParseTree::Scope* scope);
	void connect(ParseTree::Statement* statement);
	
	std::map<ParseTree::Statement*, Node*> _declarations;
	std::map<ParseTree::InlineStatement*, Edge*> _inlineValues;
	std::map<ParseTree::Identifier*, Edge*> _identifiers;
};
