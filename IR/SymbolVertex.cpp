#include "SymbolVertex.h"

string DefinitionType::toString() const
{
	switch(value)
	{
		case DefinitionType::Undefined: return L"Undefined";
		case DefinitionType::Return: return L"Return";
		case DefinitionType::Function: return L"Function";
		case DefinitionType::Argument: return L"Argument";
		default: return L"Illegal value";
	}
}

SymbolVertex::SymbolVertex(const string& identifier)
: _identifier(identifier), _definitionType(DefinitionType::Undefined), _callNode(0)
{
}

SymbolVertex::~SymbolVertex()
{
}

CallNode* SymbolVertex::callNode() const
{
	assert(definitionType() == DefinitionType::Return);
	return _callNode;
}

ClosureNode* SymbolVertex::closureNode() const
{
	assert(definitionType() == DefinitionType::Function
		|| definitionType() == DefinitionType::Argument);
	return _closureNode;
}

SymbolVertex& SymbolVertex::setUndefined()
{
	_callNode = 0;
	_definitionType = DefinitionType::Undefined;
	return *this;
}

SymbolVertex& SymbolVertex::setReturnedBy(CallNode* value)
{
	_callNode = value;
	_definitionType = DefinitionType::Return;
	return *this;
}

SymbolVertex& SymbolVertex::setFunctionOf(ClosureNode* value)
{
	_closureNode = value;
	_definitionType = DefinitionType::Function;
	return *this;
}

SymbolVertex& SymbolVertex::setArgumentOf(ClosureNode* value)
{
	_closureNode = value;
	_definitionType = DefinitionType::Argument;
	return *this;
}

std::wostream& operator<<(std::wostream& out, const SymbolVertex* s)
{
	out << L"<" << s->identifier() << L":" << s->definitionType().toString() << L">";
	return out;
}
