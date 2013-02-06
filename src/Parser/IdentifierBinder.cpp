#include "IdentifierBinder.h"

#define debug true

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
		wcerr << "===================================================================" << endl;
		wcerr << "Resolving " << identifier->name() << endl;
		wcerr << "From: ";
		identifier->parent()->print(wcerr);
		wcerr << endl;
	}
	
	ParseTree::Identifier* bindingSite = nullptr;
	
	// Find the enclosing scope
	ParseTree::Node* scopeNode = identifier;
	while(!scopeNode->isA<ParseTree::Scope>()) {
		scopeNode = scopeNode->parent();
		assert(scopeNode != nullptr);
	}
	
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
