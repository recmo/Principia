#pragma once
#include "fixups.h"
#include "Interpreter/Value.h"

class SymbolVertex;
class ClosureNode;

class Closure
{
public:
	Closure(const ClosureNode* closure, const map<SymbolVertex*,Value>& context);
	
	const ClosureNode* closure() const { return _closure; }
	map<SymbolVertex*,Value> context() const { return _context; }
	map<SymbolVertex*,Value>& context() { return _context; }
	
private:
	const ClosureNode* _closure;
	map<SymbolVertex*,Value> _context;
};
