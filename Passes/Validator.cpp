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
	if(symbol->definitionType() == DefinitionType::Undefined) return;
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
			foreach(SymbolVertex* ret, symbol->closureNode()->returns())
				causalPast(past, ret);
			break;
		case DefinitionType::Return:
			// Defined by the call and the calls arguments
			causalPast(past, symbol->callNode()->function());
			foreach(SymbolVertex* arg, symbol->callNode()->arguments())
				causalPast(past, arg);
			break;
	}
}

void Validator::causalFuture(set<const SymbolVertex*>& future, const SymbolVertex* symbol)
{
	// Prevent infinite recursion
	if(symbol->definitionType() == DefinitionType::Undefined) return;
	if(!insertSymbol(future, symbol)) return;
	
	// Find calls depending on symbol
	foreach(CallNode* call, _program->calls())
	{
		bool depends = call->function() == symbol;
		foreach(SymbolVertex* arg, call->arguments())
			if(depends |= arg == symbol)
				break;
		if(!depends) continue;
		
		// This call directly depends on symbol, so all the returns are affected
		foreach(SymbolVertex* ret, call->returns())
			causalFuture(future, ret);
	}
	
	// Find closures depending on symbol
	foreach(ClosureNode* closure, _program->closures())
	{
		bool depends = false;
		foreach(SymbolVertex* ret, closure->returns())
			if(depends |= ret == symbol) break;
		if(!depends) continue;
		
		causalFuture(future, closure->function());
		foreach(SymbolVertex* arg, closure->arguments())
			causalFuture(future, arg);
	}
}

/// Returns the minimal set of edges to evaluate for every function evaluation
set<const SymbolVertex*> Validator::internals(const ClosureNode* closure)
{
	set<const SymbolVertex*> future;
	future.insert(closure->function());
	foreach(SymbolVertex* arg, closure->arguments())
		causalFuture(future, arg);
	
	set<const SymbolVertex*> past;
	past.insert(closure->function());
	foreach(SymbolVertex* ret, closure->returns())
		causalPast(past, ret);
	
	return intersection(future, past);
}

/// Return the set of edges that could be included in the body but do not have to.
set<const SymbolVertex*> Validator::externals(const ClosureNode* closure)
{
	// Which set is this?
	// Union(causalPast(ret)) - internals ?
	
	set<const SymbolVertex*> past;
	foreach(SymbolVertex* ret, closure->returns())
		insertUnion(past, _causalPast[ret]);
	
	set<const SymbolVertex*> internal = internals(closure);
	
	return setMinus(past, internal);
}

bool Validator::isInScope(const set<const SymbolVertex*>& outerScope, const SymbolVertex* symbol)
{
	if(contains<const SymbolVertex*>(outerScope, symbol))
		return true;
	
	set<const SymbolVertex*> functionScope;
	switch(symbol->definitionType())
	{
		case DefinitionType::Undefined:
			// Externals are always global scope
			return true;
		case DefinitionType::Argument:
			// Arguments are not in scope, unless specified
			// in outerscope. (in which case we have already
			// returned true)
			return false;
		case DefinitionType::Function:
			// In scope if the function body is, except for arguments
			functionScope = outerScope;
			functionScope.insert(symbol);
			foreach(SymbolVertex* arg, symbol->closureNode()->arguments())
				functionScope.insert(arg);
			foreach(SymbolVertex* ret, symbol->closureNode()->returns())
				if(!isInScope(functionScope, ret))
					return false;
			return true;
		case DefinitionType::Return:
			// In scope if function and all the arguments are
			if(!isInScope(outerScope, symbol->callNode()->function()))
				return false;
			foreach(SymbolVertex* arg, symbol->callNode()->arguments())
				if(!isInScope(outerScope, arg))
					return false;
			return true;
	}
}

bool Validator::validate()
{
	// Find all symbols that do not depend on any argument
	set<const SymbolVertex*> imports;
	set<const SymbolVertex*> exports;
	set<const SymbolVertex*> internal;
	foreach(SymbolVertex* symbol, _program->symbols())
	{
		if(symbol->definitionType() == DefinitionType::Undefined)
			imports.insert(symbol);
		else if(isInScope(exports, symbol))
			exports.insert(symbol);
		else 
			internal.insert(symbol);
	}
	wcerr << "Imports: " << imports << endl;
	wcerr << "Exports: " << exports << endl;
	wcerr << "Internal: " << internal << endl;
	
	// Find all internals of all closures
	foreach(ClosureNode* closure, _program->closures())
	{
		set<const SymbolVertex*> finternals, fexclusive;
		fexclusive = finternals = internals(closure);
		foreach(const SymbolVertex* symbol, finternals)
			if(symbol != closure->function() && symbol->definitionType() == DefinitionType::Function)
			{
				fexclusive = setMinus(fexclusive, internals(symbol->closureNode()));
				fexclusive.insert(symbol);
			}
			
		wcerr << closure->function() << ": " << fexclusive << endl;
	}
	
	return true;
}
