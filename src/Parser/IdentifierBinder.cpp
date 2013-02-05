#include "IdentifierBinder.h"

#define debug true

void IdentifierBinder::bind()
{
	/// TODO: Global builtin identifiers
	crawl(_parseTree->topLevel());
}

void IdentifierBinder::crawl(ParseTree::Scope* scope)
{
	for(ParseTree::ScopedElement* child: scope->children()) {
		if(ParseTree::Scope* scope = dynamic_cast<ParseTree::Scope*>(child))
			crawl(scope);
		if(ParseTree::Statement* statement = dynamic_cast<ParseTree::Statement*>(child))
			crawl(statement);
		if(ParseTree::Proposition* proposition = dynamic_cast<ParseTree::Proposition*>(child))
			crawl(proposition);
	}
}

void IdentifierBinder::crawl(ParseTree::Statement* statement)
{
	for(ParseTree::Expression* expression: statement->in()) {
		if(ParseTree::Identifier* identifier = dynamic_cast<ParseTree::Identifier*>(expression))
			crawl(identifier);
		if(ParseTree::InlineStatement* statement = dynamic_cast<ParseTree::InlineStatement*>(expression))
			crawl(statement);
	}
}

void IdentifierBinder::crawl(ParseTree::Proposition* proposition)
{
	ParseTree::Expression* expression = proposition->condition();
	if(ParseTree::Identifier* identifier = dynamic_cast<ParseTree::Identifier*>(expression))
		crawl(identifier);
	if(ParseTree::InlineStatement* statement = dynamic_cast<ParseTree::InlineStatement*>(expression))
		crawl(statement);
}

void IdentifierBinder::crawl(ParseTree::Identifier* identifier)
{
	if(identifier->bindingSite())
		return;
	if(debug) {
		wcerr << endl;
		wcerr << "===================================================================" << endl;
		wcerr << "Resolving " << identifier->name() << endl;
		wcerr << "From: ";
		identifier->parent()->print(wcerr);
		wcerr << endl;
	}
	ParseTree::Identifier* bindingSite = search(identifier);
	if(bindingSite) {
		if(debug) {
			wcerr << "Resolved to " << bindingSite->name() << " from ";
			bindingSite->parent()->print(wcerr);
			wcerr << endl;
		}
		identifier->bindingSite(bindingSite);
	} else {
		wcerr << "Unresolved symbol " << identifier->name() << " from ";
		identifier->parent()->print(wcerr);
		wcerr << endl;
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
		if(debug)
			wcerr << "LEAF RIGHT2" << endl;
		if(id && id->isOutbound() && id->name() == name)
			return id;
		if(debug)
			wcerr << "LEAF RIGHT3" << endl;
	}
	if(debug)
		wcerr << "LEAF RIGHT4" << endl;
	
	// Find the enclosing scope
	/// TODO: Do not go depth first on scopes
	ParseTree::Scope* scope = getScope(identifier);
	ParseTree::ScopedElement* child = scope;
	while(scope) {
		if(debug) {
			wcerr << "SCOPE " << endl;
			scope->print(wcerr);
			wcerr << endl;
			wcerr << endl;
		}
		
		// Go left-depth-first up the scope
		ParseTree::ScopedElement* left = child;
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
		ParseTree::ScopedElement* right = child;
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
		ParseTree::Statement* parent = dynamic_cast<ParseTree::Statement*>(expression->parent());
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
		ParseTree::Statement* parent = dynamic_cast<ParseTree::Statement*>(expression->parent());
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

ParseTree::Scope* IdentifierBinder::getScope(ParseTree::Expression* expression)
{
	if(ParseTree::Statement* statement = dynamic_cast<ParseTree::Statement*>(expression->parent()))
		return statement->parent();
	if(ParseTree::Expression* expression = dynamic_cast<ParseTree::Expression*>(expression->parent()))
		return getScope(expression);
	throw L"Unkown expression type";
}

ParseTree::Identifier* IdentifierBinder::searchLeft(ParseTree::ScopedElement* sors, const string& name)
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
		ParseTree::ScopedElement* child = scope->last();
		while(child) {
			ParseTree::Identifier* id = searchLeft(child, name);
			if(id)
				return id;
			child = child->prevSibbling();
		}
	}
	
	return 0;
}

ParseTree::Identifier* IdentifierBinder::searchRight(ParseTree::ScopedElement* element, const string& name)
{
	wcerr << L"searchRight ";
	element->print(wcerr);
	wcerr << element << " " << name << endl;
	// Search through statements
	ParseTree::Statement* statement = dynamic_cast<ParseTree::Statement*>(element);
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
	ParseTree::Scope* scope = dynamic_cast<ParseTree::Scope*>(element);
	if(scope) {
		ParseTree::ScopedElement* child = scope->first();
		while(child) {
			ParseTree::Identifier* id = searchRight(child, name);
			if(id)
				return id;
			child = child->nextSibbling();
		}
	}
	
	return nullptr;
}
