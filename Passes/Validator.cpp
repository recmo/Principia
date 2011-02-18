#include "Passes/Validator.h"
#include "IR/SymbolVertex.h"
#include "IR/ClosureNode.h"
#include "IR/CallNode.h"

bool insertSymbol(set<const SymbolVertex*>& symbolSet, const SymbolVertex* symbol)
{
	if(symbolSet.find(symbol) != symbolSet.end()) return false;
	symbolSet.insert(symbol);
	return true;
}

Validator::Validator(const IntRep* program)
: _program(program)
{
}

Validator::~Validator()
{
}

/// Returns the set of symbols that this symbol depends on
void Validator::causalPast(set<const SymbolVertex*>& past, const SymbolVertex* symbol)
{
	wcerr << "causalPast(" << past << ", " << symbol << ")" << endl; 
	
	// Prevent infinite recursion
	if(!insertSymbol(past, symbol))
	{
		wcerr << "done. " << endl;
		return;
	}
	
	// Add all the nodes
	switch(symbol->definitionType())
	{
		case DefinitionType::Undefined:
			// Ignore the causes
			break;
		case DefinitionType::Argument:
			// Arguments of a closure are a first cause
			break;
		case DefinitionType::Function: {
			// Defined by the closure, so by the closed over variables
			// but lets also include internal nodes for now.
			foreach(ret, symbol->closureNode()->returns())
				causalPast(past, ret);
		} break;
		case DefinitionType::Return: {
			// Defined by the call and the calls arguments
			wcerr << "ASD: " << symbol << " " << symbol->callNode() << endl;
			causalPast(past, symbol->callNode()->function());
			wcerr << "SDA: " << symbol << " " << symbol->callNode()->arguments() << endl;
			
			foreach(arg, symbol->callNode()->arguments())
			{
				wcerr << arg << endl;
				causalPast(past, arg);
			}
		} break;
	}
	wcerr << symbol << L" new  " << past << endl; 
}

void Validator::causalFuture(set<const SymbolVertex*>& future, const SymbolVertex* symbol)
{
	// Prevent infinite recursion
	if(!insertSymbol(future, symbol)) return;
	
	// Find calls depending on symbol
	foreach(call, _program->calls())
	{
		if(call->function() != symbol)
		{
			bool depends = false;
			foreach(arg, call->arguments())
				if(depends = (arg == symbol)) break;
			if(!depends) continue;
		}
		
		// This call directly depends on symbol, so all the returns are affected
		foreach(ret, call->returns())
			causalFuture(future, ret);
	}
	
	// Find closures depending on symbol
	foreach(closure, _program->closures())
	{
		bool depends = false;
		foreach(ret, closure->returns())
			if(depends = (ret == symbol)) break;
		if(!depends) continue;
		
		causalFuture(future, closure->function());
		foreach(arg, closure->arguments())
			causalFuture(future, arg);
	}
}

/// Returns the minimal set of edges to evaluate for every function evaluation
set<const SymbolVertex*> Validator::internals(const ClosureNode* closure)
{
	set<const SymbolVertex*> future;
	foreach(arg, closure->arguments())
		insertUnion(future, _causalFuture[arg]);
	set<const SymbolVertex*> past;
	foreach(ret, closure->returns())
		insertUnion(past, _causalPast[ret]);
	return intersection(future, past);
}

/// Return the set of edges that could be included in the body but do not have to.
set<const SymbolVertex*> Validator::externals(const ClosureNode* closure)
{
	set<const SymbolVertex*> future;
	foreach(arg, closure->arguments())
		insertUnion(future, _causalFuture[arg]);
	set<const SymbolVertex*> past;
	foreach(ret, closure->returns())
		insertUnion(past, _causalPast[ret]);
	return intersection(future, past);
}

bool Validator::validate()
{
	// Calculate the causes and effects for a symbol
	foreach(symbol, _program->symbols())
	{
		wcerr << symbol << endl;
		set<const SymbolVertex*> past;
		set<const SymbolVertex*> future;
		causalPast(past, symbol);
		wcerr << symbol << L" ← " << past << endl; 
		causalFuture(future, symbol);
		wcerr << symbol << L" → " << future << endl; 
		_causalPast[symbol] = past;
		_causalFuture[symbol] = future;
	}
	
	wcerr << endl;
	foreach(closure, _program->closures())
	{
		_internal[closure] = internals(closure);
		wcerr << closure << L" : " << _internal[closure] << endl;
	}
	
	return true;
}
