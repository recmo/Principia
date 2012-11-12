#pragma once
#include <fixups.h>
using std::map;
class MetaMathStatement;

class MetaMathScope
{
public:
	MetaMathScope(MetaMathScope* parent);
	~MetaMathScope();
	
	enum SymbolKind {
		UnknownSymbol,
		Constant,
		Variable
	};
	
	enum StatementKind {
		Distinct,
		UnknownStatement,
		FloatingHypothesis,
		EssentialHypothesis,
		Axiom,
		Derived
	};
	
	SymbolKind resolveSymbolKind(const string& symbolName) const;
	StatementKind resolveStatementKind(const string& statementLabel) const;
	   const MetaMathStatement* resolveStatement(const string& statementLabel) const;
	
	void addVariable(const string& symbol);
	void addConstant(const string& symbol);
	void requireDistinct(const vector<string>& symbols);
	void floatingHypothesis(const string& label, const string& constant, const string& variable);
	void essentialHypothesis(const string& label, const vector<string>& symbols);
	void axiom(const string& label, const vector<string>& symbols);
	void derived(const string& label, const vector<string>& symbols, const vector<string>& proof);
	
	void calculateFrame(const string& label);
	
	/// @returns the essential hypothesis up to this point
	vector<MetaMathStatement*> essentialHypothesis();
	
	/// @returns the floating hypothesis declaring the type of the given variable
	MetaMathStatement* floatingHypothesis(const string& variable);
	
protected:
	MetaMathScope* _parent;
	vector<MetaMathScope*> _children;
	vector<string> _constants;
	vector<string> _variables;
	vector<MetaMathStatement*> _statements;
};
