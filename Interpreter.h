#pragma once
#include "fixups.h"
#include "Program.h"
#include "Context.h"


class Symbol;
class Closure;
class Call;

class Interpreter
{
public:
	Interpreter();
	
	void processLine(const string& line);
	
protected:
	Symbol* getOrCreateSymbol(const string& label);
	void addClosure(const string& func, const vector<string>& inputs, const vector<string>& outputs);
	void addCall(const string& func, const vector<string>& inputs, const vector<string>& outputs);
	uint64 evaluate(Symbol* symbol);
	
	Program program;
	Context context;
};
