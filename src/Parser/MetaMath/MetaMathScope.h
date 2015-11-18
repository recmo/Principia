#pragma once
#include <Unicode/string.h>
#include <vector>
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
	void requireDistinct(const std::vector<string>& symbols);
	void floatingHypothesis(const string& label, const string& constant, const string& variable);
	void essentialHypothesis(const string& label, const std::vector<string>& symbols);
	void axiom(const string& label, const std::vector<string>& symbols);
	void derived(const string& label, const std::vector<string>& symbols,
		const std::vector<string>& proof);
	
	void calculateFrame(const string& label);
	
	/// @returns the essential hypothesis up to this point
	std::vector<MetaMathStatement*> essentialHypothesis();
	
	/// @returns the floating hypothesis declaring the type of the given variable
	MetaMathStatement* floatingHypothesis(const string& variable);
	
protected:
	MetaMathScope* _parent;
	std::vector<MetaMathScope*> _children;
	std::vector<string> _constants;
	std::vector<string> _variables;
	std::vector<MetaMathStatement*> _statements;
};
