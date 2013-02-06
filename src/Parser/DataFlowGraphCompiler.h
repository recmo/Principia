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
	void declare(ParseTree::Node* node);
	void connect(ParseTree::Node* node);
	
	std::map<ParseTree::Statement*, Node*> _declarations;
	std::map<ParseTree::Statement*, Edge*> _inlineValues;
	std::map<ParseTree::Identifier*, Edge*> _identifiers;
};
