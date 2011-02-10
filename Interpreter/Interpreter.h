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
	
	Value evaluateSymbol(SymbolVertex* symbol);
	vector<Value> evaluateCall(SymbolVertex* closure, const vector<Value>& arguments);
	Value evaluateClosure(ClosureNode* closure);
	
private:
	const IntRep* _program;
	map<SymbolVertex*,Value> _context;
	
};
