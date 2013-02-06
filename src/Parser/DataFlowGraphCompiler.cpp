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
			implicitOut->name(L"·");
			outs.insert(outs.begin(), implicitOut);
		}
		
		// Create the node
		Node* node = new Node(statement->type(), ins.size(), outs.size());
		Edge* inlineValue = nullptr;
		uint i = 0;
		for(ParseTree::Identifier* out: outs) {
			node->out(i)->set(IdentifierProperty(out->name()));
			node->out(i)->set(SourceProperty(out->soureProperty()));
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
	// Propositions do not require declaration
	
	
	// Recurse on the parse tree
	for(ParseTree::Node* child: node->children())
		declare(child);
}

void DataFlowGraphCompiler::connect(ParseTree::Node* node)
{
	if(node->isA<ParseTree::Statement>()) {
		ParseTree::Statement* statement = node->to<ParseTree::Statement>();
		if(debug) {
			wcerr << "CONNECT ";
			statement->print(wcerr);
			wcerr << endl;
		}
		
		Node* node = _declarations[statement];
		for(int i = 0; i < node->in().size(); ++i) {
			ParseTree::Node* expression = statement->in()[i];
			Edge* edge = nullptr;
			
			// Connect identifiers
			if(expression->isA<ParseTree::Identifier>()) {
				ParseTree::Identifier* identifier = expression->to<ParseTree::Identifier>();
				if(identifier->bindingSite()) {
					edge = _identifiers[identifier->bindingSite()];
				} else if(contains(builtins, identifier->name())) {
					if(debug)
						wcerr << "GLOBAL " << identifier->name() << endl;
					edge = new Edge;
					edge->set(IdentifierProperty(identifier->name()));
					edge->set(ConstantProperty(Value(builtins[identifier->name()])));
				}
			}
			
			// Connect inline statements
			if(expression->isA<ParseTree::Statement>()) {
				ParseTree::Statement* inlineStatement =  expression->to<ParseTree::Statement>();
				edge = _inlineValues[inlineStatement];
				// Recursively connect
				connect(inlineStatement);
			}
			
			// Connect constants
			if(expression->isA<ParseTree::Constant>()) {
				ParseTree::Constant* constant = expression->to<ParseTree::Constant>();
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
	
	if(node->isA<ParseTree::Proposition>()) {
		ParseTree::Proposition* proposition = node->to<ParseTree::Proposition>();
		if(debug) {
			wcerr << "CONNECT ";
			proposition->print(wcerr);
			wcerr << endl;
		}
		
		/// @todo
	}
	
	// Recurse on the parse tree
	for(ParseTree::Node* child: node->children())
		connect(child);
}
