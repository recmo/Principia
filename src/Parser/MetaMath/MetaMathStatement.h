#pragma once
#include "MetaMathScope.h"
#include <fixups.h>

class MetaMathStatement
{
public:
	MetaMathStatement(MetaMathScope* parent);
	~MetaMathStatement();
	
	void kind(MetaMathScope::StatementKind value) { _kind = value; }
	MetaMathScope::StatementKind kind() const { return _kind; }
	void essentialHypothesis(vector<MetaMathStatement*> value) { _essentialHypothesis = value; }
	vector<MetaMathStatement*> essentialHypothesis() const { return _essentialHypothesis; }
	void label(const string& value) { _label = value; }
	string label() const { return _label; }
	void symbols(const vector<string>& value) { _symbols = value; }
	vector<string> symbols() const { return _symbols; }
	void proof(const vector<string>& value) { _proof = value; }
	vector<string> proof() const { return _proof; }
	
	
	vector<const MetaMathStatement*> frame() const;
	
	void verify() const;
	
protected:
	MetaMathScope* _parent;
	MetaMathScope::StatementKind _kind;
	vector<MetaMathStatement*> _essentialHypothesis;
	string _label;
	vector<string> _symbols;
	vector<string> _proof;
};

std::wostream& operator<<(std::wostream& out, const MetaMathStatement* statement);
