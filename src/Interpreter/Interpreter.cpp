#include "Interpreter.h"
#include "Closure.h"
#include "Builtins.h"
#include "DFG/DataFlowGraph.h"
#include <Parser/ConstantProperty.h>
#include "Passes/ClosureProperty.h"

bool debug = false;

vector<Value> Interpreter::evaluateFunction(const Node* closureNode, const vector<Value>& closure, const vector<Value>& arguments)
{
	if(debug)
		wcerr << closureNode << closure << arguments << endl;
	assert(closureNode);
	assert(closureNode->type() == NodeType::Closure);
	assert(closureNode->has<ClosureProperty>());
	assert(closureNode->outArrity() - 1 == arguments.size());
	const ClosureProperty& cp = closureNode->get<ClosureProperty>();
	assert(cp.edges().size() == closure.size());
	
	// Scope an interpreter
	Interpreter interpreter;
	
	// Load the closure
	const vector<const Edge*>& closureEdges = cp.edges();
	for(int i = 0; i < closureEdges.size(); ++i)
		interpreter._context[closureEdges[i]] = closure[i];
	
	// Load the arguments
	for(int i = 1; i < closureNode->outArrity(); ++i)
		interpreter._context[closureNode->out(i)] = arguments[i - 1];
	
	// Evaluate the returns
	vector<Value> returns;
	returns.reserve(closureNode->inArrity());
	for(int i = 0; i < closureNode->inArrity(); ++i)
		returns.push_back(interpreter.evaluateEdge(closureNode->in(i)));
	return returns;
}

Value Interpreter::evaluateEdge(const Edge* edge)
{
	// Constant values
	if(edge->has<ConstantProperty>())
		return edge->get<ConstantProperty>().value();
	
	// Values evaluated before
	Context::iterator contextValue = _context.find(edge);
	if(contextValue != _context.end())
		return contextValue->second;
	
	// Evaluate
	Value result;
	if(edge->source()->type() == NodeType::Closure) {
		// Node is of the form
		// f x₁ x₂ … ↦ y₁ y₂ …
		if(edge->source()->out(0) == edge) {
			// Return the closure of f
			result = Value(makeClosure(edge->source()));
		} else {
			// Cannot evaluate arguments, they need to be supplied
			// by the context
			throw "Cannot evaluate arguments";
		}
	} else if (edge->source()->type() == NodeType::Call) {
		// Node is of the form
		// y₁ y₂ … ≔ f x₁ x₂ …
		// Evaluate the function
		evaluateFunction(edge->source());
		
		// The value should now be here
		Context::iterator contextValue = _context.find(edge);
		if(contextValue != _context.end())
			return contextValue->second;
		else
			throw "Cannot evaluate return value";
	}
	
	// Store the result
	_context[edge] = result;
	return result;
}

void Interpreter::evaluateFunction(const Node* callNode)
{
	assert(callNode->type() == NodeType::Call);
	
	// Evaluate the closure
	Value value = evaluateEdge(callNode->in(0));
	assert(value.kind == Value::Builtin || value.kind == Value::Function);
	
	// Evaluate the arguments
	vector<Value> arguments;
	arguments.reserve(callNode->inArrity() - 1);
	for(int i = 1; i < callNode->inArrity(); ++i)
		arguments.push_back(evaluateEdge(callNode->in(i)));
	
	// Evaluate builtin or closure
	vector<Value> returns;
	if(value.kind == Value::Builtin) {
		
		if(debug)
			wcerr << value << arguments << endl;
		
		returns = value.builtin()(arguments);
	}
	else if(value.kind == Value::Function) {
		const Node* closureNode = value.closure()->node();
		vector<Value> closureContext = value.closure()->context();
		
		if(debug)
			wcerr << value << closureContext << arguments << endl;
		
		// Evaluate the return values
		returns = evaluateFunction(closureNode, closureContext, arguments);
	}
	
	// Store the return values in the current context
	for(int i = 0; i < callNode->outArrity(); ++i)
		_context[callNode->out(i)] = returns[i];
}

Closure* Interpreter::makeClosure(const Node* node)
{
	assert(node->type() == NodeType::Closure);
	assert(node->has<ClosureProperty>());
	if(debug)
		wcerr << "Making closure for " << node << endl;
	
	// Create the closure object
	Closure* closure = new Closure(node);
	
	// Add it to the environemnt
	// so mutual recursion will work
	_context[node->out(0)] = Value(closure);
	
	// Fill the closure object
	const vector<const Edge*> edges = node->get<ClosureProperty>().edges();
	closure->context().reserve(edges.size());
	foreach(const Edge* edge, edges)
		closure->context().push_back(evaluateEdge(edge));
	
	// Return it
	return closure;
}
