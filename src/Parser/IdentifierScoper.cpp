#include <Parser/IdentifierScoper.h>

void IdentifierScoper::scope()
{
	assert(_parseTree != nullptr);
	recurse(_parseTree->top());
}

void IdentifierScoper::recurse(ParseTree::Node* node)
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

void IdentifierScoper::recurseStatement(ParseTree::Statement* statement)
{
	assert(statement != nullptr);
	assert(!statement->isInline());
	assert(statement->parent() != nullptr);
	assert(statement->parent()->isA<ParseTree::Scope>());
	ParseTree::Scope* parent = statement->parent()->to<ParseTree::Scope>();
	if(statement->type() == NodeType::Call) {
		// Transform:
		// out₁ out₂ ≔ func in₁ in₂
		// 	<body>
		//
		// into:
		// out₁ out₂ ≔ func in₁ in₂
		// 	<body>
		// 	-in₁
		// 	-in₂
		// +out₁
		// +out₂
		
		ParseTree::Scope* body = statement->associatedScope(true);
		assert(body != nullptr);
		
		// Incoming edges
		for(ParseTree::Node* in: statement->in()) {
			if(!in->isA<ParseTree::Identifier>())
				continue;
			ParseTree::Identifier* inid = in->to<ParseTree::Identifier>();
			body->appendChild(new ParseTree::IdentifierLookup(inid));
		}
		
		// Outgoing edges
		ParseTree::Node* after = body;
		for(ParseTree::Identifier* out: statement->out()) {
			auto outVisible = new ParseTree::IdentifierVisible(out);
			parent->insertAfter(after, outVisible);
			after = outVisible;
		}
	} else {
		assert(statement->type() == NodeType::Closure);
		assert(statement->out().size() >= 1);
		
		// Transform:
		// func out₁ out₂ ↦ in₁ in₂
		// 	<body>
		//
		// into:
		// +func
		// func out₁ out₂ ↦ in₁ in₂
		// 	+out₁
		// 	+out₂
		// 	<body>
		// 	-in₁
		// 	-in₂
		
		ParseTree::Scope* body = statement->associatedScope(true);
		assert(body != nullptr);
		
		// Function edge
		auto func = statement->out()[0];
		parent->insertBefore(statement, new ParseTree::IdentifierVisible(func));
		
		// Incoming edges
		for(ParseTree::Node* in: statement->in()) {
			if(!in->isA<ParseTree::Identifier>())
				continue;
			ParseTree::Identifier* inid = in->to<ParseTree::Identifier>();
			body->appendChild(new ParseTree::IdentifierLookup(inid));
		}
		
		// Outgoing edges (except func)
		ParseTree::Node* after = nullptr;
		for(ParseTree::Identifier* out: statement->out()) {
			if(out == func)
				continue;
			auto outVisible = new ParseTree::IdentifierVisible(out);
			if(after == nullptr)
				body->insertChild(outVisible, 0);
			else
				body->insertAfter(after, outVisible);
			after = outVisible;
		}
	}
}

