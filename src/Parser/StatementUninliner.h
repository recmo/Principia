#pragma once
#include "ParseTree.h"

/// Takes a parse tree and adds dummy variables so  that all inputs are
/// identifiers.
class StatementUninliner {
public:
	StatementUninliner(ParseTree* parseTree): _parseTree(parseTree) { }
	
	void uninline();
	
private:
	ParseTree* _parseTree;
	void recurse(ParseTree::Node* node);
	void recurseStatement(ParseTree::Statement* statement);
	ParseTree::Identifier* uninlineStatement(ParseTree::Statement* statement);
};
