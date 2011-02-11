#include "Closure.h"
#include "IR/ClosureNode.h"
#include "IR/SymbolVertex.h"

Closure::Closure(const ClosureNode* closure, const map<const SymbolVertex*,Value>& context)
: _closure(closure), _context(context)
{
}
