#pragma once
#include "fixups.h"

class ClosureNode;
class CallNode;

class DefinitionType
{
public:
	DefinitionType(int v = 0) : value(v) { }
	operator int() const { return value; }
	string toString() const;
	
	const static int Undefined = 0;
	const static int Return = 1;
	const static int Function = 2;
	const static int Argument = 3;
	// const static int Constant = 3;
	
private:
	int value;
};

class SymbolVertex
{
public:
	SymbolVertex(const string& identifier);
	~SymbolVertex();
	
	string identifier() const { return _identifier; }
	DefinitionType definitionType() const { return _definitionType; };
	CallNode* callNode() const;
	ClosureNode* closureNode() const;
	
	SymbolVertex& setUndefined();
	SymbolVertex& setReturnedBy(CallNode* value);
	SymbolVertex& setFunctionOf(ClosureNode* value);
	SymbolVertex& setArgumentOf(ClosureNode* value);
	
private:
	string _identifier;
	DefinitionType _definitionType;
	union
	{
		CallNode* _callNode;
		ClosureNode* _closureNode;
	};
};

std::wostream& operator<<(std::wostream& out, const SymbolVertex* s);
