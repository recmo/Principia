#pragma once
#include "fixups.h"

class SymbolVertex;

class CallNode
{
public:
	CallNode();
	~CallNode();
	
	SymbolVertex* function() const { return _function; }
	CallNode& function(SymbolVertex* value) { _function = value; return *this; }
	vector<SymbolVertex*> arguments() const { return _arguments; }
	vector<SymbolVertex*>& arguments() { return _arguments; }
	vector<SymbolVertex*> returns() const { return _returns; }
	vector<SymbolVertex*>& returns() { return _returns; }
	
private:
	SymbolVertex* _function;
	vector<SymbolVertex*> _arguments;
	vector<SymbolVertex*> _returns;
};

std::wostream& operator<<(std::wostream& out, const CallNode* cd);

