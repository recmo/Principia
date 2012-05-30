#include "IdentifierBinder.h"

#define debug false

void IdentifierBinder::bind()
{
	/// TODO: Global builtin identifiers
	crawl(_parseTree->topLevel());
}

void IdentifierBinder::crawl(ParseTree::Scope* scope)
{
	foreach(ParseTree::StatementOrScope* child, scope->children()) {
		if(ParseTree::Scope* scope = dynamic_cast<ParseTree::Scope*>(child))
			crawl(scope);
		if(ParseTree::Statement* statement = dynamic_cast<ParseTree::Statement*>(child))
			crawl(statement);
	}
}

void IdentifierBinder::crawl(ParseTree::Statement* statement)
{
	foreach(ParseTree::Expression* expression, statement->in()) {
		if(ParseTree::Identifier* id = dynamic_cast<ParseTree::Identifier*>(expression)) {
			if(debug) {
				wcerr << endl;
				wcerr << "Resolving " << id->name() << endl;
				wcerr << "From: ";
				id->parent()->print(wcerr);
				wcerr << endl;
			}
			ParseTree::Identifier* bindingSite = search(id);
			if(bindingSite) {
				if(debug || 1) {
					wcerr << "Resolved to " << bindingSite->name() << " from ";
					bindingSite->parent()->print(wcerr);
					wcerr << endl;
				}
				id->bindingSite(bindingSite);
			} else if(debug) {
				wcerr << "Unresolved symbol " << id->name() << " from ";
				id->parent()->print(wcerr);
				wcerr << endl;
			}
		}
		if(ParseTree::InlineStatement* statement = dynamic_cast<ParseTree::InlineStatement*>(expression))
			crawl(statement);
	}
}

ParseTree::Identifier* IdentifierBinder::search(ParseTree::Identifier* identifier)
{
	if(debug)
		wcerr << "SEARCH " << identifier->name() << endl;
	string name = identifier->name();
	ParseTree::Identifier* binder = 0;
	
	// Go left-depth-first within statement
	ParseTree::Expression* sibbling = identifier;
	while(sibbling = getLeftLeaf(sibbling)) {
		if(debug) {
			wcerr << "LEAF LEFT" << endl;
			sibbling->print(wcerr);
			wcerr << endl;
		}	
		ParseTree::Identifier* id = dynamic_cast<ParseTree::Identifier*>(sibbling);
		if(id && id->isOutbound() && id->name() == name)
			return id;
	}
	
	/// TODO: Check first statement before scope
	
	// Go right-depth-first within statement
	sibbling = identifier;
	while(sibbling = getRightLeaf(sibbling)) {
		if(debug) {
			wcerr << "LEAF RIGHT" << endl;
			sibbling->print(wcerr);
			wcerr << endl;
		}
		ParseTree::Identifier* id = dynamic_cast<ParseTree::Identifier*>(sibbling);
		if(id && id->isOutbound() && id->name() == name)
			return id;
	}
	
	// Find the enclosing scope
	/// TODO: Do not go depth first on scopes
	ParseTree::StatementOrScope* child = getStatement(identifier);
	ParseTree::Scope* scope = child->parent();
	while(scope) {
		if(debug) {
			wcerr << "SCOPE " << endl;
			scope->print(wcerr);
			wcerr << endl;
			wcerr << "CHILD " << endl;
			child->print(wcerr);
			wcerr << endl;
		}
		
		// Go left-depth-first up the scope
		ParseTree::StatementOrScope* left = child;
		while(left = left->prevSibbling()) {
			if(debug) {
				wcerr << "SCOPE LEFT UP" << endl;
				left->print(wcerr);
				wcerr << endl;
			}
			
			ParseTree::Identifier* id = searchLeft(left, name);
			if(id)
				return id;
		}
			
		// Go right-depth-first down the scope
		ParseTree::StatementOrScope* right = child;
		while(right = right->nextSibbling()) {
			if(debug) {
				wcerr << "SCOPE RIGHT DOWN" << endl;
				right->print(wcerr);
				wcerr << endl;
			}
			ParseTree::Identifier* id = searchRight(right, name);
			if(id)
				return id;
		}
		
		// Go up to the enclosing parent scope
		child = scope;
		scope = scope->parent();
	}
	
	// None found
	return 0;
}

ParseTree::Expression* IdentifierBinder::getLeftLeaf(ParseTree::Expression* expression)
{
	ParseTree::Expression* left = expression->prevSibbling();
	while(!left) {
		ParseTree::Statement* parent = expression->parent();
		if(!parent)
			return 0;
		ParseTree::Expression* inlineStatement = dynamic_cast<ParseTree::InlineStatement*>(parent);
		if(!inlineStatement)
			return 0;
		left = inlineStatement->prevSibbling();
		if(left)
			break;
		expression = inlineStatement;
	}
	while(ParseTree::InlineStatement* statement = dynamic_cast<ParseTree::InlineStatement*>(left))
		left = statement->last();
	return left;
}

ParseTree::Expression* IdentifierBinder::getRightLeaf(ParseTree::Expression* expression)
{
	ParseTree::Expression* right = expression->nextSibbling();
	if(!right) {
		ParseTree::Statement* parent = expression->parent();
		if(!parent)
			return 0;
		ParseTree::Expression* inlineStatement = dynamic_cast<ParseTree::InlineStatement*>(parent);
		if(!inlineStatement)
			return 0;
		right = inlineStatement->nextSibbling();
		if(!right)
			return 0;
	}
	while(ParseTree::InlineStatement* statement = dynamic_cast<ParseTree::InlineStatement*>(right))
		right = statement->first();
	return right;
}

ParseTree::Statement* IdentifierBinder::getStatement(ParseTree::Expression* expression)
{
	ParseTree::Statement* statement = expression->parent();
	if(!statement)
		return 0;
	ParseTree::Expression* parentExpression = dynamic_cast<ParseTree::InlineStatement*>(statement);
	if(parentExpression)
		return getStatement(parentExpression);
	return statement;
}

ParseTree::Identifier* IdentifierBinder::searchLeft(ParseTree::StatementOrScope* sors, const string& name)
{
	// Search through statements
	ParseTree::Statement* statement = dynamic_cast<ParseTree::Statement*>(sors);
	if(statement) {
		ParseTree::Expression* exp = statement->last();
		while(exp) {
			ParseTree::Identifier* id = dynamic_cast<ParseTree::Identifier*>(exp);
			if(id && id->isOutbound() && id->name() == name)
				return id;
			exp = getLeftLeaf(exp);
		}
	}
	
	// Recursively search through scopes
	/// TODO: First go through statements at the scope level, then go in the scopes
	ParseTree::Scope* scope = dynamic_cast<ParseTree::Scope*>(sors);
	if(scope) {
		ParseTree::StatementOrScope* child = scope->last();
		while(child) {
			ParseTree::Identifier* id = searchLeft(child, name);
			if(id)
				return id;
			child = child->prevSibbling();
		}
	}
	
	return 0;
}

ParseTree::Identifier* IdentifierBinder::searchRight(ParseTree::StatementOrScope* sors, const string& name)
{
	// Search through statements
	ParseTree::Statement* statement = dynamic_cast<ParseTree::Statement*>(sors);
	if(statement) {
		ParseTree::Expression* exp = statement->first();
		while(exp) {
			ParseTree::Identifier* id = dynamic_cast<ParseTree::Identifier*>(exp);
			if(id && id->isOutbound() && id->name() == name)
				return id;
			exp = getRightLeaf(exp);
		}
	}
	
	// Recursively search through scopes
	/// TODO: First go through statements at the scope level, then go in the scopes
	ParseTree::Scope* scope = dynamic_cast<ParseTree::Scope*>(sors);
	if(scope) {
		ParseTree::StatementOrScope* child = scope->first();
		while(child) {
			ParseTree::Identifier* id = searchRight(child, name);
			if(id)
				return id;
			child = child->nextSibbling();
		}
	}
	
	return 0;
}
