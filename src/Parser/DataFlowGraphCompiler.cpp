#include "DataFlowGraphCompiler.h"
#include "ParseTree.h"
#include "IdentifierProperty.h"
#include "ConstantProperty.h"
#include <DFG/DataFlowGraph.h>
#include <DFG/Node.h>
#include <DFG/Edge.h>
#include <Passes/Builtins.h>

#define debug true

DataFlowGraphCompiler::DataFlowGraphCompiler(ParseTree* parseTree)
: _parseTree(parseTree)
, _dfg(0)
{
}

DataFlowGraphCompiler::~DataFlowGraphCompiler()
{
}

void DataFlowGraphCompiler::compile()
{
	_dfg = new DataFlowGraph;
	declare(_parseTree->topLevel());
	connect(_parseTree->topLevel());
}

void DataFlowGraphCompiler::declare(ParseTree::Scope* scope)
{
	foreach(ParseTree::ScopedElement* sors, scope->children()) {
		// Recurse on scopes
		ParseTree::Scope* scope = dynamic_cast<ParseTree::Scope*>(sors);
		if(scope)
			declare(scope);
		
		// Go into statements
		ParseTree::Statement* statement = dynamic_cast<ParseTree::Statement*>(sors);
		if(statement)
			declare(statement);
	}
}

void DataFlowGraphCompiler::declare(ParseTree::Statement* statement)
{
	if(debug) {
		wcerr << "DECLARE ";
		statement->print(wcerr);
		wcerr << endl;
	}
	
	ParseTree::InlineStatement* inlineStatement = dynamic_cast<ParseTree::InlineStatement*>(statement);
	int inlineValueIndex = 0;
	int outArrity = statement->out().size();
	bool hasValueMarker = false;
	
	// Check if the user specified a custom inlineValueIndex
	if(inlineStatement) for(int i = 0; i < statement->out().size(); ++i) {
		ParseTree::Identifier* id = statement->out()[i];
		if(id->name() == L"·") {
			inlineValueIndex = i;
			hasValueMarker = true;
			break;
		}
	}
	
	if(inlineStatement && !hasValueMarker)
		++outArrity;
	
	// Declare the statements outputs
	Node* node = new Node(statement->type(), statement->in().size(), outArrity);
	for(int i = 0; i < outArrity; ++i) {
		if(inlineStatement && !hasValueMarker && !i)
			continue;
		ParseTree::Identifier* id = 0;
		if(inlineStatement && !hasValueMarker && i)
			id = statement->out()[i - 1];
		else
			id = statement->out()[i];
		node->out(i)->set(IdentifierProperty(id->name()));
		node->out(i)->set(SourceProperty(id->soureProperty()));
		_identifiers[id] = node->out(i);
	}
	
	// Remember the node
	_dfg->addNode(node);
	_declarations[statement] = node;
	if(inlineStatement)
		_inlineValues[inlineStatement] = node->out(inlineValueIndex);
	
	// Recurse into any expressions
	foreach(ParseTree::Expression* expression, statement->in()) {
		ParseTree::InlineStatement* subStatement = dynamic_cast<ParseTree::InlineStatement*>(expression);
		if(subStatement)
			declare(subStatement);
	}
}

void DataFlowGraphCompiler::connect(ParseTree::Scope* scope)
{
	foreach(ParseTree::ScopedElement* sors, scope->children()) {
		// Recurse on scopes
		ParseTree::Scope* scope = dynamic_cast<ParseTree::Scope*>(sors);
		if(scope)
			connect(scope);
		
		// Go into statements
		ParseTree::Statement* statement = dynamic_cast<ParseTree::Statement*>(sors);
		if(statement)
			connect(statement);
	}
}

void DataFlowGraphCompiler::connect(ParseTree::Statement* statement)
{
	if(debug) {
		wcerr << "CONNECT ";
		statement->print(wcerr);
		wcerr << endl;
	}
	
	Node* node = _declarations[statement];
	for(int i = 0; i < node->in().size(); ++i) {
		ParseTree::Expression* expression = statement->in()[i];
		Edge* edge = 0;
		
		// Connect identifiers
		ParseTree::Identifier* identifier = dynamic_cast<ParseTree::Identifier*>(expression);
		if(identifier) {
			if(identifier->bindingSite())
				edge = _identifiers[identifier->bindingSite()];
			else if(contains(builtins, identifier->name())) {
				if(debug)
					wcerr << "GLOBAL " << identifier->name() << endl;
				edge = new Edge;
				edge->set(IdentifierProperty(identifier->name()));
				edge->set(ConstantProperty(Value(builtins[identifier->name()])));
			}
		}
		
		// Connect inline statements
		ParseTree::InlineStatement* inlineStatement = dynamic_cast<ParseTree::InlineStatement*>(expression);
		if(inlineStatement) {
			edge = _inlineValues[inlineStatement];
			// Recursively connect
			connect(inlineStatement);
		}
		
		// Connect constants
		ParseTree::Constant* constant = dynamic_cast<ParseTree::Constant*>(expression);
		if(constant) {
			edge = new Edge;
			edge->set(ConstantProperty(constant->value()));
		}
		
		// Failure
		if(edge == 0) {
			wcerr << "Could not connect expression: ";
			expression->print(wcerr);
			wcerr << endl;
			continue;
		}
		
		node->connect(i, edge);
	}
}
