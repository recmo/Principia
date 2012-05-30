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
	ParseTree::Identifier* search(ParseTree::Identifier* identifier);
	ParseTree::Expression* getLeftLeaf(ParseTree::Expression* expression);
	ParseTree::Expression* getRightLeaf(ParseTree::Expression* expression);
	ParseTree::Statement* getStatement(ParseTree::Expression* expression);
	ParseTree::Identifier* searchLeft(ParseTree::StatementOrScope* sors, const string& name);
	ParseTree::Identifier* searchRight(ParseTree::StatementOrScope* sors, const string& name);
};
