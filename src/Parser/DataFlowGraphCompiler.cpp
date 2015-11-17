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
			
			// Create the node
			NodeType type;
			if(statement->type() == ParseTree::Statement::Call)
				type = NodeType::Call;
			else
				type = NodeType::Closure;
			std::shared_ptr<Node> node = Node::make_shared(type, statement->in().size(), statement->out().size());
			if(!statement->source().isEmpty())
				node->set(statement->source());
			uint i = 0;
			for(ParseTree::Identifier* out: statement->out()) {
				node->out(i)->set(IdentifierProperty(out->name()));
				if(!out->source().isEmpty())
					node->out(i)->set(out->source());
				_identifiers[out] = node->out(i);
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
			
			std::shared_ptr<Node> node = _declarations[statement];
			for(int i = 0; i < node->in().size(); ++i) {
				std::shared_ptr<Edge> efe = edgeForExpression(statement->in()[i]);
				node->connect(i, efe);
			}
		} else {
			if(debug) {
				wcerr << "PROPOSITION ";
				statement->print(wcerr);
				wcerr << endl;
			}
			
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
			std::shared_ptr<Edge> condition = edgeForExpression(statement->firstChild());
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
		}
	}
	
	// Recurse on the parse tree
	for(ParseTree::Node* child: node->children())
		connect(child);
}

std::shared_ptr<Edge> DataFlowGraphCompiler::edgeForExpression(ParseTree::Node* expression)
{
	assert(expression);
	if(expression->isA<ParseTree::Identifier>()) {
		ParseTree::Identifier* identifier = expression->to<ParseTree::Identifier>();
		if(identifier->bindingSite()) {
			return _identifiers[identifier->bindingSite()];
		} else if(contains(builtins, identifier->name())) {
			if(debug)
				wcerr << "GLOBAL " << identifier->name() << endl;
			std::shared_ptr<Edge> edge = std::make_shared<Edge>();
			edge->set(IdentifierProperty(identifier->name()));
			edge->set(ConstantProperty(Value(builtins[identifier->name()])));
			return edge;
		}
		
		wcerr << "Could bind identifier: ";
		expression->print(wcerr);
		wcerr << endl;
		
		// Create a source free edge without a constant property
		std::shared_ptr<Edge> edge = std::make_shared<Edge>();
		edge->set(IdentifierProperty(identifier->name()));
		return edge;
	} else if(expression->isA<ParseTree::Constant>()) {
		ParseTree::Constant* constant = expression->to<ParseTree::Constant>();
		std::shared_ptr<Edge> edge = std::make_shared<Edge>();
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
