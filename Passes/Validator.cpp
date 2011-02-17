#include "Passes/Validator.h"
#include "IR/SymbolVertex.h"
#include "IR/ClosureNode.h"
#include "IR/CallNode.h"

Validator::Validator(const IntRep* program)
{
}

Validator::~Validator()
{
}

bool insertSymbol(set<const SymbolVertex*>& symbolSet, const SymbolVertex* symbol)
{
	if(symbolSet.find(symbol) != symbolSet.end()) return false;
	symbolSet.insert(symbol);
	return true;
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
		switch(symbol->definitionType())
		{
			case DefinitionType::Argument:
				// No causes
				break;
			case DefinitionType::Function:
				// Defined by the closure, so by the closed over variables
			case DefinitionType::Return:
				// Defined by the call
				break;
		}
	}
	
	// Calculate the effects of a symbol
}
