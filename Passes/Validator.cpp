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
	// Prevent infinite recursion
	if(!insertSymbol(past, symbol)) return;
	
	// Add all the nodes
	switch(symbol->definitionType())
	{
		case DefinitionType::Undefined:
			// Ignore the causes
			break;
		case DefinitionType::Argument:
			// Arguments of a closure are a first cause
			break;
		case DefinitionType::Function:
			// Defined by the closure, so by the closed over variables
			// but lets also include internal nodes for now.
			foreach(ret, symbol->closureNode()->returns())
				causalPast(past, ret);
			break;
		case DefinitionType::Return:
			// Defined by the call and the calls arguments
			causalPast(past, symbol->callNode()->function());
			foreach(arg, symbol->callNode()->arguments())
				causalPast(past, arg);
			break;
	}
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

void Validator::calculateInternalSymbols()
{
	foreach(closure, _program->closures())
	{
		set<const SymbolVertex*>& internal = _internal[closure];
		foreach(arg, closure->arguments())
			internal.insert(arg);
		
		bool changed = false;
		do {
			changed = false;
			
			// For each call, if arguments internal then returns internal
			foreach(call, _program->calls())
			{
				bool dependsOnArg = false;
				foreach(arg, call->arguments()) {
					if(internal.find(arg) != internal.end()) {
						dependsOnArg = true;
						break;
					}
				}
				if(!dependsOnArg) continue;
				foreach(ret, call->returns())
					changed |= insertSymbol(internal, ret);
			}
			
			// For each closure, if returns internal then function internal
			
			
		} while (changed);
	}
}

bool Validator::validate()
{
	// Calculate the causes for a symbol
	foreach(symbol, _program->symbols())
	{
		set<const SymbolVertex*> past;
		set<const SymbolVertex*> future;
		causalPast(past, symbol);
		causalFuture(future, symbol);
		wcerr << symbol << L" ← " << past << endl; 
		wcerr << symbol << L" → " << future << endl; 
	}
	
	// Calculate the effects of a symbol
	
	return true;
}
