#include "Parser/Parser.h"
#include "Parser/Parser.qx.h"
#include "Parser/Parser.y.h"
#include "IR/IntRep.h"
#include "IR/SymbolVertex.h"
#include "IR/CallNode.h"
#include "IR/ClosureNode.h"
#include "Interpreter/Builtins.h"

void* GrammarAlloc(void *(*mallocProc)(size_t));
void GrammarFree(void*, void (*freeProc)(void*));
void Grammar(void*, int, quex::Token*, Parser* ir);

Parser::Parser()
: _ir(new IntRep)
{
	// Create a global scope
	_scopeStack.push_back(scope());
}

Parser::Parser(IntRep* ir)
: _ir(ir)
{
}

Parser& Parser::parse(const string& filename)
{
	/// TODO: Scoping:
	///   Maintain a stack of scopes (identifier → SymbolVertex pairs)
	///     arg-symbol → look in whole stack, otherwise add to current scope as undefined
	///     ret-symbol → look in currect scope, if undefined then define, otherwise override or create
	/// TODO: Error handling
	quex::Parser lexer(encodeLocal(filename));
	quex::Token* token = 0;
	
	// Create a file level scope
	_scopeStack.push_back(scope());
	
	void* parser = GrammarAlloc(malloc);
	do {
		lexer.receive(&token);
		wcerr << "Passing token: " <<  decodeLocal(token->type_id_name());
		if(token->type_id() == TokenIdentifier || token->type_id() == TokenQuotation || token->type_id() == TokenNumber)
			wcerr << " " << token->pretty_wchar_text();
		wcerr << endl;
		Grammar(parser, token->type_id(), token, this);
	} while (token->type_id() != TokenTERMINATION);
	GrammarFree(parser, free);
	
	// Add unresolved symbols...
	scope& current = _scopeStack.back();
	for(auto i = current.begin(); i != current.end(); ++i) {
		SymbolVertex *symbol = (*i).second;
		if(symbol->definitionType() == DefinitionType::Undefined)
		{
			_ir->symbols().push_back(symbol);
		}
	}
	
	popScope();
	
	assert(_scopeStack.size() == 1);
	
	return *this;
}

void Parser::pushScope(ClosureNode* closure)
{
	wcerr << L"Push scope for " << closure->function() << endl;
	_scopeStack.push_back(scope());
	
	// Add unresolved return values to current stack
	foreach(SymbolVertex* symbol, closure->returns())
		if(symbol->definitionType() == DefinitionType::Undefined) {
			_scopeStack.back()[symbol->identifier()] = symbol;
			wcerr << L"  including " << symbol << endl;
		}
}

void Parser::popScope()
{
	wcerr << L"Pop scope" << endl;
	assert(_scopeStack.size() >= 2);
	
	// Add undefined symbols to upper scope
	scope& current = _scopeStack.back();
	scope& parent = *(_scopeStack.end() - 2);
	for(auto i = current.begin(); i != current.end(); ++i) {
		SymbolVertex *symbol = (*i).second;
		if(symbol->definitionType() == DefinitionType::Undefined)
		{
			parent[symbol->identifier()] = symbol;
			wcerr << L"  upping " << symbol << endl;
		}
	}
	_scopeStack.pop_back();
}

SymbolVertex* Parser::declareAnonymous()
{
	wcerr << L"Anonymous " << endl;
	return new SymbolVertex(L"anon");
}

SymbolVertex* Parser::declareId(const string& id)
{
	wcerr << L"Declare "  << id << endl;
	
	// See if an unresolved symbol with this id exists
	scope& current = _scopeStack.back();
	for(auto i = current.begin(); i != current.end(); ++i) {
		SymbolVertex *symbol = (*i).second;
		if(symbol->definitionType() != DefinitionType::Undefined)
			continue;
		if(symbol->identifier() == id)
			return symbol;
	}
	
	// Create a new scoped symbol
	SymbolVertex* symbol = new SymbolVertex(id);
	current[id] = symbol;
	return symbol;
}

SymbolVertex* Parser::resolveId(const string& id)
{
	wcerr << L"Resolve "  << id << endl;
	
	// See if a symbol with this id already exists
	for(auto s = _scopeStack.rbegin(); s != _scopeStack.rend(); ++s)
		for(auto i = (*s).begin(); i != (*s).end(); ++i)
			if((*i).second->identifier() == id)
				return (*i).second;
	
	// Add it to the current scope
	SymbolVertex* symbol = new SymbolVertex(id);
	_scopeStack.back()[id] = symbol;
	return symbol;
}

SymbolVertex* Parser::parseNumber(const string& litteral)
{
	wcerr << L"Parse "  << litteral << endl;
	
	SymbolVertex* symbol = new SymbolVertex(litteral);
	symbol->setConstant();
	_ir->symbols().push_back(symbol);
	return symbol;
}

SymbolVertex* Parser::stringLitteral(const string& litteral)
{
	wcerr << L"Litteral "  << litteral << endl;
	SymbolVertex* symbol = new SymbolVertex(litteral);
	symbol->setConstant();
	_ir->symbols().push_back(symbol);
	return symbol;
}

CallNode* Parser::createCall(SymbolVertex* function, const vector<SymbolVertex*>& rets, const vector<SymbolVertex*>& args)
{
	wcerr << L"Call " << rets << function << args << endl;
	
	// Verify that outputs are undefined
	foreach(SymbolVertex* ret, rets)
		assert(ret->definitionType() == DefinitionType::Undefined);
	
	// Create the call
	CallNode* call = new CallNode;
	call->function(function);
	foreach(SymbolVertex* arg, args)
		call->arguments().push_back(arg);
	foreach(SymbolVertex* ret, rets)
		call->returns().push_back(ret);
	
	// Store in IR structure
	_ir->calls().push_back(call);
	foreach(SymbolVertex* ret, rets) {
		_ir->symbols().push_back(ret);
		ret->setReturnedBy(call);
	}
	
	return call;
}

ClosureNode* Parser::createClosure(SymbolVertex* function, const vector<SymbolVertex*>& rets, const vector<SymbolVertex*>& args)
{
	wcerr << L"Close " << rets << function << args << endl;
	
	// Verify that outputs are undefined
	assert(function->definitionType() == DefinitionType::Undefined)
	foreach(SymbolVertex* arg, args)
		assert(arg->definitionType() == DefinitionType::Undefined);
	
	// Create the closure
	ClosureNode* closure = new ClosureNode;
	closure->function(function);
	foreach(SymbolVertex* arg, args)
		closure->arguments().push_back(arg);
	foreach(SymbolVertex* ret, rets)
		closure->returns().push_back(ret);
	
	// Store in IR structure
	_ir->closures().push_back(closure);
	function->setFunctionOf(closure);
	_ir->symbols().push_back(function);
	foreach(SymbolVertex* arg, args) {
		_ir->symbols().push_back(arg);
		arg->setArgumentOf(closure);
	}
	
	return closure;
}
