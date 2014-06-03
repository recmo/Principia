#pragma once
#include "ParseTree.h"

/// Takes a parse tree and adds implicit outputs · where they are missing in
/// inline statements
class ImplicitOutputAdder {
public:
	ImplicitOutputAdder(ParseTree* parseTree): _parseTree(parseTree) { }
	
	void addImplicitOutputs();
	
private:
	ParseTree* _parseTree;
	void recurse(ParseTree::Node* node);
	void addImplicit(ParseTree::Statement* statement);
};
