#include "IdentifierBinder.h"

#define debug false

void IdentifierBinder::bind()
{
	/// TODO: Global builtin identifiers
	recurse(_parseTree->top());
}

void IdentifierBinder::recurse(ParseTree::Node* node)
{
	if(node->isA<ParseTree::Identifier>())
		bind(node->to<ParseTree::Identifier>());
	for(ParseTree::Node* child: node->children())
		recurse(child);
}

void IdentifierBinder::bind(ParseTree::Identifier* identifier)
{
	if(identifier->outbound())
		return;
	if(identifier->bindingSite())
		return;
	if(debug) {
		wcerr << endl;
		wcerr << "---------------------------------------------------" << endl;
		wcerr << "Resolving " << identifier << endl;
		wcerr << "From: " << identifier->parent() << endl;
	}
	
	ParseTree::Identifier* site = bindingSite(identifier);
	
	// Set the binding site if we found one
	if(site) {
		if(debug) {
			wcerr << "Resolved to " << site->name() << " from ";
			site->parent()->print(wcerr);
			wcerr << endl;
		}
		identifier->bindingSite(site);
	} else {
		wcerr << "Unresolved symbol " << identifier->name() << " from ";
		identifier->parent()->print(wcerr);
		wcerr << endl;
	}
}

ParseTree::Identifier* IdentifierBinder::bindingSite(ParseTree::Identifier* identifier)
{
	// Find the enclosing line and scope
	ParseTree::Node* line = identifier;
	assert(line->parent());
	while(!line->parent()->isA<ParseTree::Scope>()) {
		line = line->parent();
		assert(line->parent());
	}
	ParseTree::Node* scope = line->parent();
	
	// Search a child (sibling) scope first in bottom to top order
	ParseTree::Node* childScope = line->nextSibbling();
	if(childScope && !childScope->isA<ParseTree::Scope>())
		childScope = nullptr;
	if(debug) {
		wcerr << "Scope: " << scope << endl;
		wcerr << "SibScope: " << childScope << endl;
	}
	if(childScope) {
		for(ParseTree::Node* node = childScope->lastChild(); node; node = node->prevSibbling()) {
			ParseTree::Identifier* site = directBind(node, identifier->name());
			if(site)
				return site;
		}
	}
	
	{ // Search line from to the left
		ParseTree::Node* left = identifier;
		while(left) {
			left = leftOf(line, left);
			if(left && left->isA<ParseTree::Identifier>()) {
				ParseTree::Identifier* leftId = left->to<ParseTree::Identifier>();
				if(leftId->outbound() && leftId->name() == identifier->name())
					return leftId;
			}
		}
	}
	
	{ // Search line to the right
		ParseTree::Node* right = identifier;
		while(right) {
			right = rightOf(line, right);
			if(right && right->isA<ParseTree::Identifier>()) {
				ParseTree::Identifier* rightId = right->to<ParseTree::Identifier>();
				if(rightId->outbound() && rightId->name() == identifier->name())
					return rightId;
			}
		}
	}
	
	// Search the scope
	while(scope) {
		// Search up in scope
		// Sibling scopes get to see all the associated closures nodes
		for(ParseTree::Node* node = line->prevSibbling(); node; node = node->prevSibbling()) {
			bool fullClosure = (line->isA<ParseTree::Scope>()) && (node == line->prevSibbling());
			ParseTree::Identifier* site = directBind(node, identifier->name(), fullClosure);
			if(site)
				return site;
		}
		
		// Search down in scope
		for(ParseTree::Node* node = line->nextSibbling(); node; node = node->nextSibbling()) {
			ParseTree::Identifier* site = directBind(node, identifier->name());
			if(site)
				return site;
		}
		
		// Go up in scope
		scope = scope->parent();
		line = line->parent();
	}
	
	return nullptr;
}

ParseTree::Identifier* IdentifierBinder::directBind(ParseTree::Node* node, const string& name, bool fullClosure) const
{
	if(!node->isA<ParseTree::Statement>())
		return nullptr;
	
	ParseTree::Statement* statement = node->to<ParseTree::Statement>();
	if(statement->type() == NodeType::Call || fullClosure) {
		// Iterate over all the outputs
		for(ParseTree::Identifier* identifier: statement->out())
			if(identifier->name() == name)
				return identifier;
	} else {
		// Iterate only over the closure output
		ParseTree::Identifier* identifier = statement->out()[0];
		if(identifier->name() == name)
			return identifier;
	}
	return nullptr;
}

// Do a walk of the leaves
ParseTree::Node* IdentifierBinder::leftOf(ParseTree::Node* top, ParseTree::Node* node)
{
	// Go left
	ParseTree::Node* sibling = node->prevSibbling();
	while(!sibling) {
		// Go up until we have a sibling
		node = node->parent();
		if(!node || node == top)
			return nullptr;
		sibling = node->prevSibbling();
	}
	node = sibling;
	
	// Go deep
	while(node->lastChild())
		node = node->lastChild();
	
	return node;
}

// Do a walk of the leaves
ParseTree::Node* IdentifierBinder::rightOf(ParseTree::Node* top, ParseTree::Node* node)
{
	// Go left
	ParseTree::Node* sibling = node->nextSibbling();
	while(!sibling) {
		// Go up until we have a sibling
		node = node->parent();
		if(!node || node == top)
			return nullptr;
		sibling = node->nextSibbling();
	}
	node = sibling;
	
	// Go deep
	while(node->firstChild())
		node = node->firstChild();
	
	return node;
}
