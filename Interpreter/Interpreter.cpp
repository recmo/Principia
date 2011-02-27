#include "Interpreter.h"
#include "IR/SymbolVertex.h"
#include "IR/CallNode.h"
#include "IR/ClosureNode.h"
#include "Closure.h"
#include "Builtins.h"

static const bool verbose = false;

Interpreter::Interpreter(const IntRep* program)
: _program(program)
{
	// Link all undefined symbols
	foreach(SymbolVertex* symbol, _program->symbols())
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
	if(verbose) wcerr << "Retrieving " << symbol << endl;
	
	// Has it already been evaluated?
	if(contains<const SymbolVertex*,Value>(_context, symbol))
		return _context[symbol];
	
	if(verbose) wcerr << "Evaluating " << symbol << endl;
	
	// Evaluate depending on how
	switch(symbol->definitionType())
	{
		case DefinitionType::Return: {
			vector<Value> args;
			foreach(SymbolVertex* arg, symbol->callNode()->arguments())
				args.push_back(evaluateSymbol(arg));
			vector<Value> rets = evaluateFunction(symbol->callNode()->function(), args);
			int ret_index = 0;
			foreach(SymbolVertex* ret, symbol->callNode()->returns())
			{
				if(verbose) wcerr << ret << " = " << rets[ret_index] << endl;
				_context[ret] = rets[ret_index++];
			}
			return _context[symbol];
		}
		case DefinitionType::Function:
		{
			Value v = evaluateClosure(symbol->closureNode());
			if(verbose) wcerr << symbol << " = " << v << endl;
			return _context[symbol] = v;
		}
		case DefinitionType::Argument:
		case DefinitionType::Undefined:
		default:
			wcerr << "When evaluating " << symbol->identifier() << endl;
			throw std::runtime_error("Could not evaluate symbol.");
	}
}

vector<Value> Interpreter::evaluateFunction(const SymbolVertex* functionSymbol, const std::vector< Value >& arguments)
{
	Value function = evaluateSymbol(functionSymbol);
	if(verbose) wcerr << "Calling " << functionSymbol->identifier();
	if(verbose) wcerr << " = " << function;
	if(verbose) wcerr << " " << arguments << endl;
	if(function.kind == Value::Function)
	{
		if(verbose) wcerr << "... is a closure " <<  function.function()->closure()->function()->identifier() << endl;
		return evaluateClosureCall(function.function(), arguments);
	}
	else if(function.kind == Value::Builtin)
	{
		if(verbose) wcerr << "... is a builtin" << endl;
		return function.builtin()(arguments);
	}
	else
	{
		wcerr << "When calling " << functionSymbol << endl;
		throw "Could not evaluate call";
	}
}

vector<Value> Interpreter::evaluateClosureCall(const Closure* closure, const vector<Value>& arguments)
{
	// Create an execution context
	map<const SymbolVertex*, Value> old_context = _context;
	_context = closure->context();
	
	// Add the arguments
	assert(arguments.size() == closure->closure()->arguments().size());
	int arg_index = 0;
	foreach(SymbolVertex* arg, closure->closure()->arguments())
	{
		if(verbose) wcerr << "  " << arg << " = " << arguments[arg_index] << endl;
		_context[arg] = arguments[arg_index++];
	}
	
	if(verbose) wcerr << " executing context = " << _context << endl;
	
	// Evaluate the return values
	/// TODO: randomize order
	vector<Value> returns;
	foreach(SymbolVertex* ret, closure->closure()->returns())
	{
		if(verbose) wcerr << " evaluating " << ret << endl;
		returns.push_back(evaluateSymbol(ret));
	}
	
	_context = old_context;
	return returns;
}

Value Interpreter::evaluateClosure(const ClosureNode* closureNode)
{
	/// TODO: Prune unused symbols
	Closure* closure = new Closure(closureNode, _context);
	closure->context()[closureNode->function()] = closure; // for recursive functions
	if(verbose) wcerr << "Creating closure for " << closureNode->function()->identifier() << endl;
	if(verbose) wcerr << " context = " << closure->context() << endl;
	return closure;
}
