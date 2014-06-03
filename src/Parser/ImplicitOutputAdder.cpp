#include <Parser/ImplicitOutputAdder.h>

void ImplicitOutputAdder::addImplicitOutputs()
{
	assert(_parseTree != nullptr);
	recurse(_parseTree->top());
}

void ImplicitOutputAdder::recurse(ParseTree::Node* node)
{
	assert(node != nullptr);
	
	ParseTree::Node* child = node->firstChild();
	while(child != nullptr) {
		if(child->isA<ParseTree::Statement>()) {
			ParseTree::Statement* statement = child->to<ParseTree::Statement>();
			if(statement->isInline())
				addImplicit(statement);
		}
		recurse(child);
		child = child->nextSibbling();
	}
}

void ImplicitOutputAdder::addImplicit(ParseTree::Statement* statement)
{
	assert(statement != nullptr);
	assert(statement->isInline());
	
	// Check if there already is an implicit output
	for(ParseTree::Node* child: statement->children()) {
		if(!child->isA<ParseTree::Identifier>())
			continue;
		ParseTree::Identifier* identifier = child->to<ParseTree::Identifier>();
		if(identifier->name() == L"·")
			return;
	}
	
	// Add one on the front
	ParseTree::Identifier* identifier = new ParseTree::Identifier;
	identifier->name(L"·");
	identifier->setOutbound();
	statement->insertChild(identifier, 0);
}
