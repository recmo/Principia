#include "SymbolVertex.h"
#include "ClosureNode.h"
#include "CallNode.h"

string DefinitionType::toString() const
{
	switch(value)
	{
		case DefinitionType::Undefined: return L"Undefined";
		case DefinitionType::Return: return L"Return";
		case DefinitionType::Function: return L"Function";
		case DefinitionType::Argument: return L"Argument";
		case DefinitionType::Constant: return L"Constant";
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

SymbolVertex* SymbolVertex::function() const
{
	switch(definitionType()) {
		case DefinitionType::Argument: return _callNode->function();
		case DefinitionType::Function: return _callNode->function();
		case DefinitionType::Return: return _closureNode->function();
		case DefinitionType::Undefined:
		default:	assert(false);
	}
}

Value* SymbolVertex::constant() const
{
	assert(definitionType() == DefinitionType::Constant);
	return _constant;
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

SymbolVertex& SymbolVertex::setConstant(Value* constant)
{
	_definitionType = DefinitionType::Constant;
	_constant = constant;
}

std::wostream& operator<<(std::wostream& out, const SymbolVertex* s)
{
	// out << L"<" << s->identifier() << L":" << s->definitionType().toString() << L">";
	out << s->identifier();
	return out;
}
