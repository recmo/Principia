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
	ParseTree::Node* sibScope = line->nextSibbling();
	if(sibScope && !sibScope->isA<ParseTree::Scope>())
		sibScope = nullptr;
	if(debug) {
		wcerr << "Scope: " << scope << endl;
		wcerr << "SibScope: " << sibScope << endl;
	}
	
	// Search a sibling scope first in bottom to top order
	if(sibScope) {
		for(ParseTree::Node* node = sibScope->lastChild(); node; node = node->prevSibbling()) {
			ParseTree::Identifier* site = directBind(node, identifier->name());
			if(site)
				return site;
		}
	}
	
	// Search the line itself
	/// @todo Also check inline statements
	{
		ParseTree::Identifier* site = directBind(line, identifier->name(), true);
		if(site)
			return site;
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

