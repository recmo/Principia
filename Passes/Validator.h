#pragma once
#include "fixups.h"
#include <IR/IntRep.h>

///@brief Verifies the correctness of the IR
class Validator
{
public:
	Validator(const IntRep* program);
	~Validator();
	
	void calculateInternalSymbols();
	
	bool validate();
	void causalPast(std::set<const SymbolVertex*>& past, const SymbolVertex* symbol);
	void causalFuture(std::set<const SymbolVertex*>& future, const SymbolVertex* symbol);
	set<const SymbolVertex*> internals(const ClosureNode* closure);
	set<const SymbolVertex*> externals(const ClosureNode* closure);
	
private:
	const IntRep* _program;
	map<const ClosureNode*, set<const SymbolVertex*> > _internal;
	map<const ClosureNode*, set<const SymbolVertex*> > _closure;
	map<const SymbolVertex*, set<const SymbolVertex*> > _causalFuture;
	map<const SymbolVertex*, set<const SymbolVertex*> > _causalPast;
};
