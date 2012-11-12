#include "MetaMathStatement.h"

MetaMathStatement::MetaMathStatement(MetaMathScope* parent)
: _parent(parent)
, _kind(MetaMathScope::UnknownStatement)
, _essentialHypothesis()
, _label()
, _symbols()
, _proof()
{
}

MetaMathStatement::~MetaMathStatement()
{
}

vector<const MetaMathStatement*> MetaMathStatement::frame() const
{
	vector<const MetaMathStatement*> result;
	
	// Collect the relevant variables
	vector<string> variables;
	auto process = [&] (const MetaMathStatement* statement) {
		for(const string& symbol: statement->symbols()) {
			if(_parent->resolveSymbolKind(symbol) != MetaMathScope::Variable)
				continue;
			if(std::find(variables.begin(), variables.end(), symbol) != variables.end())
				continue;
			variables.push_back(symbol);
		}
	};
	for(MetaMathStatement* statement: _essentialHypothesis)
		process(statement);
	process(this);
	
	// Add the corresponding floating hypothesis to the frame
	for(const string& variable: variables)
		result.push_back(_parent->floatingHypothesis(variable));
	
	// Add all the essential hypothesis to the frame
	for(MetaMathStatement* statement: _essentialHypothesis)
		result.push_back(statement);
	
	// We do not add the statement itself
	return result;
}

void MetaMathStatement::verify() const
{
	assert(_kind == MetaMathScope::Derived);
	vector<vector<string>> stack;
	for(const string& label: _proof) {
		const MetaMathStatement* statement = _parent->resolveStatement(label);
		assert(statement);
		if(statement->kind() == MetaMathScope::FloatingHypothesis || statement->kind() == MetaMathScope::EssentialHypothesis) {
			stack.push_back(statement->symbols());
		} else if(statement->kind() == MetaMathScope::Axiom || statement->kind() == MetaMathScope::Derived) {
			
			vector<const MetaMathStatement*> frame = statement->frame();
			assert(stack.size() >= frame.size());
			map<string, vector<string>> substitution;
			/// @todo generate the mapping and assert the correctness!
			
			/// @todo Check distinct variables
			
			for(int i = 0; i < frame.size(); ++i)
				stack.pop_back();
			
			vector<string> result;
			for(const string& symbol: statement->symbols())
				for(const string& subtitutedSymbol: substitution[symbol])
					result.push_back(subtitutedSymbol);
			stack.push_back(result);
		}
		
		wcout << stack << endl;
	}
}

std::wostream& operator<<(std::wostream& out, const MetaMathStatement* statement)
{
	out << L"(";
	out << statement->label();
	switch(statement->kind()) {
		case MetaMathScope::Distinct: out << L" $d "; break;
		case MetaMathScope::FloatingHypothesis: out << L" $f "; break;
		case MetaMathScope::EssentialHypothesis: out << L" $e "; break;
		case MetaMathScope::Axiom: out << L" $a "; break;
		case MetaMathScope::Derived: out << L" $p "; break;
	}
	for(const string& symbol: statement->symbols()) {
		out << symbol << L" ";
	}
	out << L")";
	return out;
}

