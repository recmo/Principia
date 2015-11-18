#pragma once
#include <Parser/MetaMath/MetaMathScope.h>
#include <Unicode/string.h>
#include <vector>

class MetaMathStatement
{
public:
	MetaMathStatement(MetaMathScope* parent);
	~MetaMathStatement();
	
	void kind(MetaMathScope::StatementKind value) { _kind = value; }
	MetaMathScope::StatementKind kind() const { return _kind; }
	void essentialHypothesis(std::vector<MetaMathStatement*> value)
	{ _essentialHypothesis = value; }
	std::vector<MetaMathStatement*> essentialHypothesis() const
	{ return _essentialHypothesis; }
	void label(const string& value) { _label = value; }
	const string& label() const { return _label; }
	void symbols(const std::vector<string>& value) { _symbols = value; }
	const std::vector<string>& symbols() const { return _symbols; }
	void proof(const std::vector<string>& value) { _proof = value; }
	const std::vector<string>& proof() const { return _proof; }
	
	std::vector<const MetaMathStatement*> frame() const { return _frame; }
	
	void calcFrame();
	
	void verify() const;
	
protected:
	MetaMathScope* _parent;
	uint _declarationOrder;
	MetaMathScope::StatementKind _kind;
	std::vector<MetaMathStatement*> _essentialHypothesis;
	std::vector<const MetaMathStatement*> _frame;
	string _label;
	std::vector<string> _symbols;
	std::vector<string> _proof;
	
	static uint declarationCounter();
};

std::wostream& operator<<(std::wostream& out, const MetaMathStatement* statement);
