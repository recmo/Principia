#pragma once
#include "ParseTree.h"

// Finds the bindingSite of all the identifiers. The parse tree must not
// contain any inline statements.
class IdentifierBinder {
public:
	IdentifierBinder(ParseTree* parseTree): _parseTree(parseTree) { }
	
	void bind();
	
private:
	ParseTree* _parseTree;
	void recurse(ParseTree::Node* node);
	void bind(ParseTree::IdentifierLookup* lookup);
	ParseTree::IdentifierVisible* find(const std::wstring& name, ParseTree::Node* node);
};
