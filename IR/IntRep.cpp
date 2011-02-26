#include "IntRep.h"
#include "IR/SymbolVertex.h"
#include "IR/CallNode.h"
#include "IR/ClosureNode.h"

IntRep::IntRep()
{
}

IntRep::~IntRep()
{
	// IntRep is owner of all its objects
	foreach(CallNode* call, _calls) delete call;
	foreach(ClosureNode* closure, _closures) delete closure;
	foreach(SymbolVertex* symbol, _symbols) delete symbol;
}
