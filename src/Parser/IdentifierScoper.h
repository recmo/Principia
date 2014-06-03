#pragma once
#include "ParseTree.h"

/// Takes a parse tree and adds IdentifierVisible and IdentifierLookup  variables so  that all inputs are
/// identifiers.
class IdentifierScoper {
public:
	IdentifierScoper(ParseTree* parseTree): _parseTree(parseTree) { }
	
	void scope();
	
private:
	ParseTree* _parseTree;
	void recurse(ParseTree::Node* node);
	void recurseStatement(ParseTree::Statement* statement);
};
