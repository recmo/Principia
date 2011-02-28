#include "Parser.h"
#include "IR/IntRep.h"
#include "IR/SymbolVertex.h"
#include "IR/CallNode.h"
#include "IR/ClosureNode.h"

Parser::Parser(IntRep* program)
: _program(program)
{
}

Parser::~Parser()
{
}

void Parser::parseLine(const string& line)
{
	vector<string> tokens = tokenize(line);
	if(tokens.empty()) return;
	
	// Parse <outputs> "≔" <func> <inputs>
	auto call_sign = find(tokens.begin(), tokens.end(), L"≔");
	if(call_sign != tokens.end())
	{
		if(call_sign + 1 == tokens.end())
			throw std::runtime_error("Call without function");
		CallNode* call = new CallNode;
		call->function(symbolFromIdentifier(*(call_sign + 1)));
		for(auto arg = call_sign + 2; arg != tokens.end(); ++arg)
			call->arguments().push_back(symbolFromIdentifier(*arg));
		for(auto ret = tokens.begin(); ret != call_sign; ++ret)
			call->returns().push_back(&(symbolFromIdentifier(*ret)->setReturnedBy(call)));
		_program->calls().push_back(call);
		return;
	}
	
	// Parse <func> <arguments> ↦ <returns>
	auto closure_sign = find(tokens.begin(), tokens.end(), L"↦");
	if(closure_sign != tokens.end())
	{
		if(closure_sign == tokens.begin())
			throw std::runtime_error("Closure should have at least a function name");
		ClosureNode* closure = new ClosureNode;
		closure->function(&(symbolFromIdentifier(*(tokens.begin()))->setFunctionOf(closure)));
		for(auto arg = tokens.begin() + 1; arg != closure_sign; ++arg)
			closure->arguments().push_back(&(symbolFromIdentifier(*arg)->setArgumentOf(closure)));
		for(auto ret = closure_sign + 1; ret != tokens.end(); ++ret)
			closure->returns().push_back(symbolFromIdentifier(*ret));
		_program->closures().push_back(closure);
		return;
	}
	
	// Parse <prefix> ⊧ <substitution>
	auto prefix_sign = find(tokens.begin(), tokens.end(), L"⊧");
	if(prefix_sign != tokens.end())
	{
		if(tokens.begin() + 1 != prefix_sign)
			throw std::runtime_error("Prefix definition requires single prefix and substitution.");
		if(tokens.size() == 2)
			prefixes.erase(tokens[0]);
		else
			prefixes[tokens[0]] = prefixDecode(tokens[2]);
		return;
	}
	
	// Nothing matched!
	wcerr << endl << line << endl; 
	throw std::runtime_error("Error: Could not parse line.");
}

string Parser::prefixDecode(const string& identifier)
{
	typedef std::map<string, string> map_type;
	foreach(map_type::value_type& pattern, prefixes)
		if(startsWith(pattern.first, identifier))
			return pattern.second + identifier.substr(pattern.first.size(), identifier.size());
	return identifier;
}

SymbolVertex* Parser::symbolFromIdentifier(const string& identifier, bool autoCreate)
{
	string full_name = prefixDecode(identifier);
	foreach(SymbolVertex* symbol, _program->symbols())
		if(symbol->identifier() == full_name)
			return symbol;
	if(autoCreate) {
		SymbolVertex *symbol = new SymbolVertex(full_name);
		_program->symbols().push_back(symbol);
		return symbol;
	}
	return null;
}
