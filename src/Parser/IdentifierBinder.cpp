#include "IdentifierBinder.h"
#include <DFG/Builtin.h>
#include <iostream>
using std::wcerr;
using std::endl;

#define debug true

void IdentifierBinder::bind()
{
	/// TODO: Global builtin identifiers
	recurse(_parseTree->top());
}

void IdentifierBinder::recurse(ParseTree::Node* node)
{
	if(node->isA<ParseTree::IdentifierLookup>())
		bind(node->to<ParseTree::IdentifierLookup>());
	for(ParseTree::Node* child: node->children())
		recurse(child);
}

void IdentifierBinder::bind(ParseTree::IdentifierLookup* lookup)
{
	assert(lookup != nullptr);
	assert(lookup->identifier() != nullptr);
	assert(lookup->identifier()->inbound());
	auto visible = find(lookup->identifier()->name(), lookup);
	if(visible == nullptr) {
		// Check builtins
		/* Todo
		if(contains(builtins, lookup->identifier()->name())) {
			return;
		}
		*/
		
		string name = lookup->identifier()->name();
		
		Value value = Builtin::lookup(name);
		if(value.type() != Value::None) {
			lookup->identifier()->replaceWith(new ParseTree::Constant(value));
		} else {
			wcerr << "Could not find symbol: " << lookup->identifier()->name() << endl;
		}
		return;
	}
	assert(visible->identifier() != nullptr);
	assert(visible->identifier()->outbound());
	lookup->identifier()->bindingSite(visible->identifier());
}

ParseTree::IdentifierVisible* IdentifierBinder::find(const std::wstring& name, ParseTree::Node* node)
{
	if(node == nullptr)
		return nullptr;
	
	// Look up
	auto sibbling = node->prevSibbling();
	while(sibbling != nullptr) {
		if(sibbling->isA<ParseTree::IdentifierVisible>()) {
			auto iv = sibbling->to<ParseTree::IdentifierVisible>();
			if(iv->identifier()->name() == name)
				return iv;
		}
		sibbling = sibbling->prevSibbling();
	}
	
	// Look down
	sibbling = node->nextSibbling();
	while(sibbling != nullptr) {
		if(sibbling->isA<ParseTree::IdentifierVisible>()) {
			auto iv = sibbling->to<ParseTree::IdentifierVisible>();
			if(iv->identifier()->name() == name)
				return iv;
		}
		sibbling = sibbling->nextSibbling();
	}
	
	// Go to parent
	return find(name, node->parent());
}
