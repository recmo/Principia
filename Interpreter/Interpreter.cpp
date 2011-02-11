#include "Interpreter.h"
#include "IR/SymbolVertex.h"
#include "IR/CallNode.h"
#include "IR/ClosureNode.h"
#include "Closure.h"
#include "Builtins.h"

Interpreter::Interpreter(const IntRep* program)
: _program(program)
{
	// Link all undefined symbols
	foreach(symbol, _program->symbols())
	{
		sint64 integer = 0;
		double real = 0.0;
		BuiltinFunction builtin = 0;
		if(symbol->definitionType() != DefinitionType::Undefined)
			continue;
		else if(parse<sint64>(symbol->identifier(), integer))
			_context[symbol] = integer;
		else if(parse<double>(symbol->identifier(), real))
			_context[symbol] = real;
		else if(tryGet<string, BuiltinFunction>(builtins, symbol->identifier(), builtin))
			_context[symbol] = builtin;
		else
			throw "Could not find symbol";
	}
}

Interpreter::~Interpreter()
{
}

Value Interpreter::evaluateSymbol(const SymbolVertex* symbol)
{
	// Has it already been evaluated?
	if(contains<const SymbolVertex*,Value>(_context, symbol))
		return _context[symbol];
	
	// Evaluate depending on how
	switch(symbol->definitionType())
	{
		case DefinitionType::Return: {
			vector<Value> args;
			foreach(arg, symbol->callNode()->arguments())
				args.push_back(evaluateSymbol(arg));
			vector<Value> rets = evaluateFunction(symbol->callNode()->function(), args);
			foreach(ret, symbol->callNode()->returns())
				_context[ret] = rets[ret_index];
			return _context[symbol];
		}
		case DefinitionType::Function:
			return _context[symbol] = evaluateClosure(symbol->closureNode());
		case DefinitionType::Argument:
		case DefinitionType::Undefined:
		default:
			wcerr << symbol->identifier() << endl;
			throw std::runtime_error("Could not evaluate symbol.");
	}
}

vector<Value> Interpreter::evaluateFunction(const SymbolVertex* functionSymbol, const std::vector< Value >& arguments)
{
	Value function = evaluateSymbol(functionSymbol);
	if(function.kind == Value::Function)
		return evaluateClosureCall(function.function(), arguments);
	else if(function.kind == Value::Builtin)
		return function.builtin()(arguments);
	else
		throw "Could not evaluate call";
}

vector<Value> Interpreter::evaluateClosureCall(const Closure* closure, const vector<Value>& arguments)
{
	// Create an execution context
	map<const SymbolVertex*, Value> old_context = _context;
	_context = closure->context();
	
	// Add the arguments
	assert(arguments.size() == closure->closure()->arguments().size());
	foreach(arg, closure->closure()->arguments())
		_context[arg] = arguments[arg_index];
	
	// Evaluate the return values
	/// TODO: randomize order
	vector<Value> returns;
	foreach(ret, closure->closure()->returns())
		returns.push_back(evaluateSymbol(ret));
	
	_context = old_context;
	return returns;
}

Value Interpreter::evaluateClosure(const ClosureNode* closureNode)
{
	/// TODO: Prune unused symbols
	Closure* closure = new Closure(closureNode, _context);
	closure->context()[closureNode->function()] = closure; // for recursive functions
	return closure;
}
