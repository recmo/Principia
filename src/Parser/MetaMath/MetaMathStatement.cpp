#include "MetaMathStatement.h"

uint MetaMathStatement::declarationCounter()
{
	static uint counter = 0;
	return counter++;
}

MetaMathStatement::MetaMathStatement(MetaMathScope* parent)
: _parent(parent)
, _declarationOrder(declarationCounter())
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

void MetaMathStatement::calcFrame()
{
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
	/// @todo sort by declaration order!!!
	for(const string& variable: variables)
		_frame.push_back(_parent->floatingHypothesis(variable));
	
	// Sort floating hypothesis by declaration order
	std::sort(_frame.begin(), _frame.end(), [] (const MetaMathStatement* a, const MetaMathStatement* b) {
		return a->_declarationOrder < b->_declarationOrder;
	});
	
	// Add all the essential hypothesis to the frame
	for(MetaMathStatement* statement: _essentialHypothesis)
		_frame.push_back(statement);
	
	// We do not add the statement itself
}

void MetaMathStatement::verify() const
{
	wcout << _label << "\t";
	string previous;
	for(const string& symbol: symbols()) {
		if(previous != L"(" && symbol != L")")
			wcout << " ";
		wcout << symbol;
		previous = symbol;
	}
	wcout << endl;
	
	assert(_kind == MetaMathScope::Derived);
	vector<vector<string>> stack;
	for(const string& label: _proof) {
		const MetaMathStatement* statement = _parent->resolveStatement(label);
		if(!statement)
			wcerr << L"Could not find " << label << endl;
		assert(statement);
		if(statement->kind() == MetaMathScope::FloatingHypothesis || statement->kind() == MetaMathScope::EssentialHypothesis) {
			stack.push_back(statement->symbols());
		} else if(statement->kind() == MetaMathScope::Axiom || statement->kind() == MetaMathScope::Derived) {
			
			// wcout << "Applying theorem " << statement->label() << " " << statement->symbols() << endl;
			vector<const MetaMathStatement*> frame = statement->frame();
			// wcout << frame << endl;
			assert(stack.size() >= frame.size());
			map<string, vector<string>> substitution;
			
			// generate the mapping
			for(int i = 0; i < frame.size(); ++i) {
				const MetaMathStatement* frameStatement = frame[i];
				vector<string> stackStatement = stack[stack.size() - frame.size() + i];
				if(frameStatement->kind() == MetaMathScope::FloatingHypothesis) {
					// wcout << "Match " << frameStatement << " to " << stackStatement << endl;
					assert(frameStatement->symbols()[0] == stackStatement[0]);
					vector<string> substitute;
					for(int i = 1; i < stackStatement.size(); ++i)
						substitute.push_back(stackStatement[i]);
					substitution[frameStatement->symbols()[1]] = substitute;
				} else if (frameStatement->kind() == MetaMathScope::EssentialHypothesis) {
					vector<string> transformedFrame;
					for(const string& symbol: frameStatement->symbols()) {
						if(substitution.find(symbol) == substitution.end())
							transformedFrame.push_back(symbol);
						else for(const string& substituteSymbol: substitution[symbol])
							transformedFrame.push_back(substituteSymbol);
					}
					//wcout << "Check " << frameStatement->symbols() << " as " << transformedFrame << " to " << stackStatement << endl;
					if(transformedFrame != stackStatement)
						wcout << "Error " << transformedFrame << "  =/=   " << stackStatement << endl;
					assert(transformedFrame == stackStatement);
				}
			}
			
			/// @todo Check distinct variables
			
			// Pop the hypothesis off the stack
			for(int i = 0; i < frame.size(); ++i)
				stack.pop_back();
			
			// Push the theorem on the stack
			vector<string> result;
			for(const string& symbol: statement->symbols()) {
				if(substitution.find(symbol) != substitution.end())
					for(const string& subtitutedSymbol: substitution[symbol])
						result.push_back(subtitutedSymbol);
				else
					result.push_back(symbol);
			}
			stack.push_back(result);
		}
		
		// wcout << stack << endl;
	}
	
// 	// Verify that the proof stack now only contains our theorem
	assert(stack.size() == 1);
	assert(stack[0] == symbols());
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

