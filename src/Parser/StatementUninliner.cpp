#include <Parser/StatementUninliner.h>

void StatementUninliner::uninline()
{
	assert(_parseTree != nullptr);
	recurse(_parseTree->top());
}

void StatementUninliner::recurse(ParseTree::Node* node)
{
	assert(node != nullptr);
	
	if(node->isA<ParseTree::Statement>())
		recurseStatement(node->to<ParseTree::Statement>());
	
	if(node->isA<ParseTree::Scope>()) {
		ParseTree::Node* child = node->firstChild();
		while(child != nullptr) {
			recurse(child); // This call can change the list of children
			child = child->nextSibbling();
		}
	}
}

void StatementUninliner::recurseStatement(ParseTree::Statement* statement)
{
	assert(statement != nullptr);
	assert(!statement->isInline());
	
	// Loop backwards over the children, uninlining them
	ParseTree::Node* child = statement->lastChild();
	while(child != nullptr) {
		if(child->isA<ParseTree::Statement>()) {
			ParseTree::Statement* childStatement = child->to<ParseTree::Statement>();
			ParseTree::Identifier* identifier = uninlineStatement(childStatement);
			assert(identifier->inbound());
			
			// Replace the inline statement with the identifier
			statement->swapChild(childStatement, identifier);
			child = identifier;
			assert(childStatement->parent() == nullptr);
			
			// Add the inline statement to the scope to make it uninlined
			ParseTree::Scope* statementScope = statement->associatedScope(true);
			assert(statementScope != nullptr);
			statementScope->insertChild(childStatement, 0);
		}
		child = child->prevSibbling();
	}
}

ParseTree::Identifier* StatementUninliner::uninlineStatement(ParseTree::Statement* statement)
{
	assert(statement != nullptr);
	assert(statement->isInline());
	
	// Create a unique name of the form ·<number>
	static uint id = 0;
	std::wstringstream nameStream;
	nameStream << L"·";
	nameStream << ++id;
	const std::wstring name = nameStream.str();
	
	// Create the replacement identifier;
	ParseTree::Identifier* outIdentifier = new ParseTree::Identifier;
	outIdentifier->name(name);
	outIdentifier->setOutbound();
	
	// Replace · with out new identifier
	for(ParseTree::Node* node: statement->children()) {
		if(node->isA<ParseTree::Identifier>()) {
			ParseTree::Identifier* identifier = node->to<ParseTree::Identifier>();
			if(identifier->name() == L"·") {
				statement->swapChild(node, outIdentifier);
				break;
			}
		}
	}
	
	// Create the replacement identifier;
	ParseTree::Identifier* inIdentifier = new ParseTree::Identifier;
	inIdentifier->name(name);
	inIdentifier->setInbound();
	return inIdentifier;
}
