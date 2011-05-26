#pragma once
#include "fixups.h"

class Call;
class SymbolVertex;
class CallNode;
class ClosureNode;

///@brief Intermediate Representation
class IntRep
{
public:
	IntRep();
	~IntRep();
	
	vector<ClosureNode*> closures() const { return _closures; }
	vector<ClosureNode*>& closures() { return _closures; }
	vector<CallNode*> calls() const { return _calls; }
	vector<CallNode*>& calls() { return _calls; }
	vector<SymbolVertex*> symbols() const { return _symbols; }
	vector<SymbolVertex*>& symbols() { return _symbols; }
	
private:
	vector<ClosureNode*> _closures;
	vector<CallNode*> _calls;
	vector<SymbolVertex*> _symbols;
};