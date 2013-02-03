#include "MetaMathScope.h"
#include "MetaMathStatement.h"

MetaMathScope::MetaMathScope(MetaMathScope* parent)
: _parent(parent)
, _children()
, _constants()
, _variables()
, _statements()
{
	if(_parent)
		_parent->_children.push_back(this);
}

MetaMathScope::~MetaMathScope()
{
	for(MetaMathScope* scope: _children)
		delete scope;
}

MetaMathScope::SymbolKind MetaMathScope::resolveSymbolKind(const string& symbolName) const
{
	for(const string& constant: _constants)
		if(constant == symbolName)
			return Constant;
	for(const string& variable: _variables)
		if(variable == symbolName)
			return Variable;
	if(_parent)
		return _parent->resolveSymbolKind(symbolName);
	return UnknownSymbol;
}

MetaMathScope::StatementKind MetaMathScope::resolveStatementKind(const string& statementLabel) const
{
	for(const MetaMathStatement* statement: _statements)
		if(statement->label() == statementLabel)
			return statement->kind();
	if(_parent)
		return _parent->resolveStatementKind(statementLabel);
	return UnknownStatement;
}

const MetaMathStatement* MetaMathScope::resolveStatement(const string& statementLabel) const
{
	for(const MetaMathStatement* statement: _statements)
		if(statement->label() == statementLabel)
			return statement;
	if(_parent)
		return _parent->resolveStatement(statementLabel);
	return nullptr;
}

void MetaMathScope::addVariable(const string& symbol)
{
	// Must be undeclared
	assert(resolveSymbolKind(symbol) == UnknownSymbol);
	_variables.push_back(symbol);
}

void MetaMathScope::addConstant(const string& symbol)
{
	// Must be undeclared
	assert(resolveSymbolKind(symbol) == UnknownSymbol);
	
	// Must be top scope
	assert(_parent == 0);
	_constants.push_back(symbol);
}


void MetaMathScope::requireDistinct(const vector<string>& symbols)
{
	// Symbols must be variables
	for(const string& symbol: symbols)
		assert(resolveSymbolKind(symbol) == Variable);
	
	assert(symbols.size() >= 2);
	MetaMathStatement* statement = new MetaMathStatement(this);
	statement->kind(Distinct);
	statement->symbols(symbols);
	_statements.push_back(statement);
}

void MetaMathScope::floatingHypothesis(const string& label, const string& constant, const string& variable)
{
	assert(resolveSymbolKind(constant) == Constant);
	assert(resolveSymbolKind(variable) == Variable);
	vector<string> symbols;
	symbols.push_back(constant);
	symbols.push_back(variable);
	MetaMathStatement* statement = new MetaMathStatement(this);
	statement->label(label);
	statement->kind(FloatingHypothesis);
	statement->symbols(symbols);
	_statements.push_back(statement);
}

void MetaMathScope::essentialHypothesis(const string& label, const vector<string>& symbols)
{
	assert(symbols.size() >= 2);
	assert(resolveSymbolKind(symbols[0]) == Constant);
	MetaMathStatement* statement = new MetaMathStatement(this);
	statement->label(label);
	statement->kind(EssentialHypothesis);
	statement->symbols(symbols);
	_statements.push_back(statement);
}

void MetaMathScope::axiom(const string& label, const vector<string>& symbols)
{
	assert(symbols.size() >= 2);
	assert(resolveSymbolKind(symbols[0]) == Constant);
	MetaMathStatement* statement = new MetaMathStatement(this);
	statement->label(label);
	statement->kind(Axiom);
	statement->essentialHypothesis(essentialHypothesis());
	statement->symbols(symbols);
	statement->calcFrame();
	_statements.push_back(statement);
	
	// Axioms also go in the global scope
	MetaMathScope* top = this;
	while(top->_parent)
		top = top->_parent;
	top->_statements.push_back(statement);
}

void MetaMathScope::derived(const string& label, const vector<string>& symbols, const vector<string>& proof)
{
	assert(symbols.size() >= 2);
	assert(resolveSymbolKind(symbols[0]) == Constant);
	MetaMathStatement* statement = new MetaMathStatement(this);
	statement->kind(Derived);
	statement->essentialHypothesis(essentialHypothesis());
	statement->label(label);
	statement->symbols(symbols);
	statement->proof(proof);
	statement->calcFrame();
	_statements.push_back(statement);
	
	// Derivations also go in the global scope
	MetaMathScope* top = this;
	while(top->_parent)
		top = top->_parent;
	top->_statements.push_back(statement);
	
	// Verify proof!
	statement->verify();
}

vector<MetaMathStatement*> MetaMathScope::essentialHypothesis()
{
	vector<MetaMathStatement*> result;
	if(_parent)
		result = _parent->essentialHypothesis();
	for(MetaMathStatement* statement: _statements)
		if(statement->kind() == EssentialHypothesis)
			result.push_back(statement);
	return result;
}

MetaMathStatement* MetaMathScope::floatingHypothesis(const string& variable)
{
	for(MetaMathStatement* statement: _statements)
		if(statement->kind() == FloatingHypothesis && statement->symbols()[1] == variable)
			return statement;
	if(_parent)
		return _parent->floatingHypothesis(variable);
	return nullptr;
}
