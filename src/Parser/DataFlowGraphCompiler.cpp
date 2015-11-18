#include "DataFlowGraphCompiler.h"
#include <Parser/ParseTree.h>
#include <Parser/IdentifierProperty.h>
#include <DFG/DataFlowGraph.h>
#include <DFG/ConstantProperty.h>
#include <DFG/Builtin.h>
#include <DFG/PropositionProperty.h>
#include <Utilities/exceptions.h>
#include <Utilities/assert.h>
#include <iostream>
using std::wcerr;
using std::endl;

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

// Declare pass, creates nodes for statements
void DataFlowGraphCompiler::declare(ParseTree::Node* node)
{
	if(node->isA<ParseTree::Statement>()) {
		ParseTree::Statement* statement = node->to<ParseTree::Statement>();
		if(statement->type() == ParseTree::Statement::Call
			|| statement->type() == ParseTree::Statement::Closure) {
			
			if(debug) {
				wcerr << "DECLARE ";
				statement->print(wcerr);
				wcerr << endl;
			}
			
			// Node type
			const Node::Type type =
				(statement->type() == ParseTree::Statement::Call)
				? Node::Call : Node::Closure;
			
			// Node arity
			const uint outArity = statement->out().size();
			const uint inArity = statement->in().size();
			
			// Create the node
			std::shared_ptr<Node> node = std::make_shared<Node>(type, outArity, inArity);
			
			// Set the node SourceProperty
			if(!statement->source().isEmpty())
				node->set(statement->source());
			
			// Set the output IdentifierProperty and SourceProperty
			uint i = 0;
			for(ParseTree::Identifier* out: statement->out()) {
				node->out(i).set(IdentifierProperty(out->name()));
				if(!out->source().isEmpty())
					node->out(i).set(out->source());
				++i;
			}
			
			// Set the input IdentifierProperty and SourceProperty
			i = 0;
			for(ParseTree::Node* in: statement->in()) {
				if(in->isA<ParseTree::Identifier>()) {
					node->in(i).set(IdentifierProperty{
						in->to<ParseTree::Identifier>()->name()
					});
				}
				if(!in->source().isEmpty()) {
					node->in(i).set(in->source());
				}
				++i;
			}
			
			// Store the output identifier mapping
			i = 0;
			for(ParseTree::Identifier* out: statement->out()) {
				_identifiers[out] = node->out(i).shared_from_this();
				++i;
			}
			
			// Remember the node
			_dfg->addNode(node);
			_declarations[statement] = node;
		}
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
		
		if(statement->type() == ParseTree::Statement::Call
		|| statement->type() == ParseTree::Statement::Closure) {
			if(debug) {
				wcerr << "CONNECT ";
				statement->print(wcerr);
				wcerr << endl;
			}
			
			// Connect all the inputs
			assert(contains(_declarations, statement));
			std::shared_ptr<Node> node = _declarations[statement];
			for(int i = 0; i < node->inArity(); ++i) {
				ParseTree::Node* expression = statement->in()[i];
				
				// Bind identifiers
				if(expression->isA<ParseTree::Identifier>()) {
					ParseTree::Identifier* identifier = expression->to<ParseTree::Identifier>();
					assert(identifier->bindingSite() != nullptr);
					assert(contains(_identifiers, identifier->bindingSite()));
					node->in(i) << *_identifiers[identifier->bindingSite()];
					
				} else if(expression->isA<ParseTree::Constant>()) {
					// Constant values
					node->in(i) << expression->to<ParseTree::Constant>()->value();
					
				} else {
					wcerr << endl << endl << __LINE__ << endl;
					throw unimplemented{};
				}
			}
		} else {
			if(debug) {
				wcerr << "PROPOSITION ";
				statement->print(wcerr);
				wcerr << endl;
			}
			
			wcerr << endl << endl << __LINE__ << endl;
			throw unimplemented{};
			
			/*
			assert(statement->out().size() == 0);
			assert(statement->in().size() == 1);
			
			// Find the statement associated with the proposition
			ParseTree::Scope* scope = statement->enclosingScope();
			assert(scope);
			ParseTree::Statement* associate = scope->associatedStatement();
			if(!statement) {
				wcerr << "No statement associated to scope for proposition " << statement << endl;
				return;
			}
			std::shared_ptr<Node> target = _declarations[associate];
			assert(target);
			std::shared_ptr<OutPort> condition = outForExpression(statement->firstChild());
			assert(condition);
			PropositionProperty pp;
			if(target->has<PropositionProperty>())
				pp = target->get<PropositionProperty>();
			switch(statement->type()) {
				case ParseTree::Statement::Precondition: pp.precondition(condition); break;
				case ParseTree::Statement::Axiom: pp.axiom(condition); break;
				case ParseTree::Statement::Assertion: pp.assertion(condition); break;
				case ParseTree::Statement::Postcondition: pp.postcondition(condition); break;
				default:
					throw L"Unimplement statement type";
			}
			target->set(pp);
			*/
		}
	}
	
	// Recurse on the parse tree
	for(ParseTree::Node* child: node->children())
		connect(child);
}
