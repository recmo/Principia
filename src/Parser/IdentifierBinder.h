#pragma once
#include "ParseTree.h"

class IdentifierBinder {
public:
	IdentifierBinder(ParseTree* parseTree): _parseTree(parseTree) { }
	
	void bind();
	
private:
	ParseTree* _parseTree;
	void recurse(ParseTree::Node* node);
	void bind(ParseTree::Identifier* identifier);
	ParseTree::Identifier* bindingSite(ParseTree::Identifier* identifier);
	ParseTree::Identifier* directBind(ParseTree::Node* node, const string& name, bool fullClosure = false) const;
	ParseTree::Node* leftOf(ParseTree::Node* top, ParseTree::Node* node);
	ParseTree::Node* rightOf(ParseTree::Node* top, ParseTree::Node* node);
};
