#include "DataFlowGraphCompiler.h"
#include "ParseTree.h"
#include "IdentifierProperty.h"
#include "ConstantProperty.h"
#include <DFG/DataFlowGraph.h>
#include <DFG/Node.h>
#include <DFG/Edge.h>
#include <Passes/Builtins.h>
#include <Verifier/PropositionProperty.h>

#define debug false

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
	declare(_parseTree->top());
	connect(_parseTree->top());
}

void DataFlowGraphCompiler::declare(ParseTree::Node* node)
{
	if(node->isA<ParseTree::Statement>()) {
		ParseTree::Statement* statement = node->to<ParseTree::Statement>();
		if(debug) {
			wcerr << "DECLARE ";
			statement->print(wcerr);
			wcerr << endl;
		}
		
		// Sort inputs and outputs
		std::vector<ParseTree::Identifier*> outs;
		std::vector<ParseTree::Node*> ins;
		bool customOutDot = false;
		for(ParseTree::Node* child: statement->children()) {
			if(child->isA<ParseTree::Identifier>()) {
				ParseTree::Identifier* identifier = child->to<ParseTree::Identifier>();
				if(identifier->outbound()) {
					outs.push_back(identifier);
					if(identifier->name() == L"·")
						customOutDot = true;
				} else {
					ins.push_back(child);
				}
			} else {
				ins.push_back(child);
			}
		}
		if(statement->isInline() && !customOutDot) {
			ParseTree::Identifier* implicitOut = new ParseTree::Identifier();
			if(!statement->source().isEmpty())
				implicitOut->source(statement->source());
			implicitOut->name(L"·");
			outs.insert(outs.begin(), implicitOut);
		}
		
		// Create the node
		Node* node = new Node(statement->type(), ins.size(), outs.size());
		if(!statement->source().isEmpty())
			node->set(statement->source());
		Edge* inlineValue = nullptr;
		uint i = 0;
		for(ParseTree::Identifier* out: outs) {
			node->out(i)->set(IdentifierProperty(out->name()));
			if(!out->source().isEmpty());
				node->out(i)->set(out->source());
			_identifiers[out] = node->out(i);
			if(statement->isInline() && out->name() == L"·")
				inlineValue = node->out(i);
			++i;
		}
		
		// Remember the node
		_dfg->addNode(node);
		_declarations[statement] = node;
		if(inlineValue)
			_inlineValues[statement] = inlineValue;
	}
	
	// Recurse on the parse tree
	for(ParseTree::Node* child: node->children())
		declare(child);
}

void DataFlowGraphCompiler::connect(ParseTree::Node* node)
{
	assert(node);
	if(node->isA<ParseTree::Statement>()) {
		ParseTree::Statement* statement = node->to<ParseTree::Statement>();
		if(debug) {
			wcerr << "CONNECT ";
			statement->print(wcerr);
			wcerr << endl;
		}
		
		Node* node = _declarations[statement];
		for(int i = 0; i < node->in().size(); ++i) {
			Edge* efe = edgeForExpression(statement->in()[i]);
			node->connect(i, efe);
		}
	} else if(node->isA<ParseTree::Proposition>()) {
		ParseTree::Proposition* proposition = node->to<ParseTree::Proposition>();
		if(debug) {
			wcerr << "PROPOSITION ";
			proposition->print(wcerr);
			wcerr << endl;
		}
		
		// Find the statement associated with the proposition
		ParseTree::Scope* scope = proposition->enclosingScope();
		assert(scope);
		ParseTree::Statement* statement = scope->associatedStatement();
		if(!statement) {
			wcerr << "No statement associated to scope for proposition " << proposition << endl;
			return;
		}
		Node* target = _declarations[statement];
		assert(target);
		Edge* condition = edgeForExpression(proposition->firstChild());
		assert(condition);
		PropositionProperty pp;
		if(target->has<PropositionProperty>())
			pp = target->get<PropositionProperty>();
		switch(proposition->kind()) {
		case ParseTree::Proposition::Precondition: pp.precondition(condition); break;
		case ParseTree::Proposition::Axiom: pp.axiom(condition); break;
		case ParseTree::Proposition::Assertion: pp.assertion(condition); break;
		case ParseTree::Proposition::Postcondition: pp.postcondition(condition); break;
		}
		target->set(pp);
		
	}
	
	// Recurse on the parse tree
	for(ParseTree::Node* child: node->children())
		connect(child);
}

Edge* DataFlowGraphCompiler::edgeForExpression(ParseTree::Node* expression)
{
	assert(expression);
	if(expression->isA<ParseTree::Identifier>()) {
		ParseTree::Identifier* identifier = expression->to<ParseTree::Identifier>();
		if(identifier->bindingSite()) {
			return _identifiers[identifier->bindingSite()];
		} else if(contains(builtins, identifier->name())) {
			if(debug)
				wcerr << "GLOBAL " << identifier->name() << endl;
			Edge* edge = new Edge(nullptr);
			edge->set(IdentifierProperty(identifier->name()));
			edge->set(ConstantProperty(Value(builtins[identifier->name()])));
			return edge;
		}
		wcerr << "Could bind identifier: ";
		expression->print(wcerr);
		wcerr << endl;
		throw L"Could not bind identifier";
	} else if(expression->isA<ParseTree::Statement>()) {
		ParseTree::Statement* inlineStatement =  expression->to<ParseTree::Statement>();
		return _inlineValues[inlineStatement];
	} else if(expression->isA<ParseTree::Constant>()) {
		ParseTree::Constant* constant = expression->to<ParseTree::Constant>();
		Edge* edge = new Edge(nullptr);
		if(!constant->source().isEmpty())
			edge->set(constant->source());
		edge->set(ConstantProperty(constant->value()));
		return edge;
	}
	wcerr << "Could not connect expression: ";
	expression->print(wcerr);
	wcerr << endl;
	throw L"Could not connect expression";
}
