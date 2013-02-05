#pragma once
#include "ParseTree.h"

class IdentifierBinder {
public:
	IdentifierBinder(ParseTree* parseTree): _parseTree(parseTree) { }
	
	void bind();
	
private:
	ParseTree* _parseTree;
	void crawl(ParseTree::Scope* scope);
	void crawl(ParseTree::Statement* statement);
	void crawl(ParseTree::Proposition* proposition);
	void crawl(ParseTree::Identifier* identifier);
	ParseTree::Identifier* search(ParseTree::Identifier* identifier);
	ParseTree::Expression* getLeftLeaf(ParseTree::Expression* expression);
	ParseTree::Expression* getRightLeaf(ParseTree::Expression* expression);
	ParseTree::Scope* getScope(ParseTree::Expression* expression);
	ParseTree::Identifier* searchLeft(ParseTree::ScopedElement* sors, const string& name);
	ParseTree::Identifier* searchRight(ParseTree::ScopedElement* sors, const string& name);
};
