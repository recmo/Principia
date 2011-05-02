#pragma once
#include "fixups.h"
#include "IR/IntRep.h"
#include "Interpreter/Value.h"

class ClosureNode;
class Closure;

class Interpreter
{
public:
	Interpreter(const IntRep* program);
	~Interpreter();
	
	Value evaluateSymbol(const SymbolVertex* symbol);
	vector<Value> evaluateFunction(const SymbolVertex* function, const vector<Value>& arguments);
	vector<Value> evaluateClosureCall(const Closure* closure, const vector<Value>& arguments);
	Value evaluateClosure(const ClosureNode* closure);
	
private:
	const IntRep* _program;
	map<const SymbolVertex*, Value> _context;
	
};
