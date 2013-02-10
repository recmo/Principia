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
		wcerr << "Resolving " << identifier << endl;
		wcerr << "From: " << identifier->parent() << endl;
	}
	
	/// @todo If the statement is immediately follows by a scope, then search this scope before the statements enclosing scope
	
	ParseTree::Identifier* bindingSite = nullptr;
	
	// Find the enclosing scope
	ParseTree::Node* scope = identifier;
	while(!scope->isA<ParseTree::Scope>()) {
		scope = scope->parent();
		assert(scope != nullptr);
	}
	
	// Search, alternating between left/up, right/down and broadening the scope
	ParseTree::Node* left = identifier;
	ParseTree::Node* right = identifier;
	for(;;) {
		
		// Search left/up
		for(;;) {
			// Go up until we have a prevSibbling
			while(left != scope && !left->prevSibbling())
				left = left->parent();
			if(left == scope)
				break;
			
			// Go left and find rightmost leaf
			/// @todo Do not go into closures other than first argument
			/// @todo Do not go into scopes
			left = left->prevSibbling();
			while(left->lastChild())
				left = left->lastChild();
			
			// Check if we have the binding site
			if(left->isA<ParseTree::Identifier>()) {
				ParseTree::Identifier* id = left->to<ParseTree::Identifier>();
				if(id->outbound() && id->name() == identifier->name()) {
					bindingSite = id;
					break;
				}
			}
		}
		if(bindingSite)
			break;
		
		// Search right/down
		for(;;) {
			// Go up until we have a nextSibbling
			while(right != scope && !right->nextSibbling())
				right = right->parent();
			if(right == scope)
				break;
			
			// Go right and find leftmost leaf
			/// @todo Do not go into closures other than first argument
			/// @todo Do not go into scopes
			right = right->nextSibbling();
			while(right->firstChild())
				right = right->firstChild();
			
			// Check if we have the binding site
			if(right->isA<ParseTree::Identifier>()) {
				ParseTree::Identifier* id = right->to<ParseTree::Identifier>();
				if(id->outbound() && id->name() == identifier->name()) {
					bindingSite = id;
					break;
				}
			}
		}
		if(bindingSite)
			break;
		
		// Broaden the scope
		if(!scope) {
			// Already at broadest scope
			break;
		}
		scope = scope->parent();
	}
	
	// Set the binding site if we found one
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
