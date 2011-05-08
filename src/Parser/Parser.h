#pragma once
#include <fixups.h>
#include <IR/IntRep.h>

class Parser
{
public:
	Parser();
	Parser(IntRep* ir);
	
	const IntRep* ir() const { return _ir; }
	IntRep* ir() { return _ir; }
	
	Parser& parse(const string& filename);
	Parser& parse(std::wifstream input);
	
// For parser implementation:
	void pushScope(ClosureNode* closure);
	void popScope();
	SymbolVertex* declareAnonymous();
	SymbolVertex* declareId(const string& id);
	SymbolVertex* resolveId(const string& id);
	SymbolVertex* parseNumber(const string& litteral);
	SymbolVertex* stringLitteral(const string& litteral);
	CallNode* createCall(SymbolVertex* function, const std::vector< SymbolVertex* >& rets, const std::vector< SymbolVertex* >& args);
	ClosureNode* createClosure(SymbolVertex* function, const std::vector< SymbolVertex* >& rets, const std::vector< SymbolVertex* >& args);
	
private:
	typedef map<string, SymbolVertex*> scope;
	IntRep* _ir;
	vector<scope> _scopeStack;
};


