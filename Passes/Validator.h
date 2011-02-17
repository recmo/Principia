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
	
private:
	const IntRep* _program;
	map<const ClosureNode*, set<const SymbolVertex*> > _internal;
	map<const SymbolVertex*, set<const SymbolVertex*> > _causes;
	map<const SymbolVertex*, set<const SymbolVertex*> > _effects;
};
