#include "Interpreter.h"
#include "IR/SymbolVertex.h"
#include "IR/CallNode.h"
#include "IR/ClosureNode.h"
#include "Closure.h"

Interpreter::Interpreter(const IntRep* program)
: _program(program)
{
}

Interpreter::~Interpreter()
{
}

Value Interpreter::evaluateSymbol(SymbolVertex* symbol)
{
	// Has it already been evaluated?
	if(contains<SymbolVertex*,Value>(_context, symbol))
		return _context[symbol];
	
	// TODO: Builtins!
	
	// Evaluate depending on how
	switch(symbol->definitionType())
	{
		case DefinitionType::Return: {
			vector<Value> args;
			foreach(arg, symbol->callNode()->arguments())
				args.push_back(evaluateSymbol(arg));
			vector<Value> rets;
			rets = evaluateCall(symbol->callNode()->function(), args);
			foreach(ret, symbol->callNode()->returns())
				_context[ret] = rets[ret_index];
			return _context[symbol];
		}
		case DefinitionType::Function:
			return _context[symbol] = evaluateClosure(symbol->closureNode());
		case DefinitionType::Argument:
		case DefinitionType::Undefined:
		default: throw std::runtime_error("Could not evaluate symbol.");
	}
}

vector<Value> Interpreter::evaluateCall(SymbolVertex* callSymbol, const vector<Value>& arguments)
{
	// Find the closureNode
	Value closureValue = evaluateSymbol(callSymbol);
	const Closure* closure = closureValue.function();
	
	// Create an execution context
	map<SymbolVertex*,Value> old_context = _context;
	_context = closure->context();
	
	// Add the arguments
	assert(arguments.size() == closure->closure()->arguments().size());
	foreach(arg, closure->closure()->arguments())
		_context[arg] = arguments[arg_index];
	
	// Evaluate the return values
	vector<Value> returns;
	foreach(ret, closure->closure()->returns())
		returns.push_back(evaluateSymbol(ret));
	
	_context = old_context;
	return returns;
}

Value Interpreter::evaluateClosure(ClosureNode* closureNode)
{
	// TODO: Prune unused symbols
	return new Closure(closureNode, _context);
}
