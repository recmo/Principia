#include "MetaMathParser.h"
#include "MetaMathScope.h"
#include <Utilities/assert.h>
#include <Unicode/convert.h>
#include <iostream>
#include <fstream>

using std::vector;

MetaMathParser::MetaMathParser(const string& filename)
: _filename(filename)
, _globalScope(new MetaMathScope(nullptr))
, _scopeStack()
{
	_scopeStack.push_back(_globalScope);
}

MetaMathParser::~MetaMathParser()
{
	delete _globalScope;
}

void MetaMathParser::parse()
{
	std::wifstream in(encodeLocal(_filename));
	string token;
	string label;
	string operation;
	vector<string> arguments;
	vector<string> proof;
	while(in.good()) {
		in >> token;
		token = improveToken(token);
		
		// Commensts $( ... $)
		if(token == L"$(") {
			while(in.good() && token != L"$)")
				in >> token;
			
		// File inclusion $[ filename $]
		} else if(token == L"$[") {
			throw L"File inclusion is not supported";
			
		// Scoping statements: ${ begins a block
		} else if(token == L"${") {
			pushScope();
			
		// Scoping statements: $} ends a block
		} else if (token == L"$}") {
			popScope();
			
		// End of statement
		} else if (token == L"$.") {
			if(operation == L"$=")
				operation = L"$p";
			handleStatement(label, operation, arguments, proof);
			label.clear();
			operation.clear();
			arguments.clear();
			proof.clear();
			
		// If the operation is $= accumulate the proof
		} else if (operation == L"$=") {
			proof.push_back(token);
			
		// Start of statement
		} else if(token[0] == '$') {
				operation = token;
			
		// If no operation has been declared yet, this is a label
		} else if(operation.empty()) {
			label = token;
			
		// Or else accumulate arguments
		} else {
			arguments.push_back(token);
		}
	}
}

MetaMathScope* MetaMathParser::currentScope()
{
	return _scopeStack.back();
}

void MetaMathParser::pushScope()
{
	MetaMathScope* scope = new MetaMathScope(currentScope());
	_scopeStack.push_back(scope);
}

void MetaMathParser::popScope()
{
	assert(currentScope() != _globalScope);
	_scopeStack.pop_back();
}

void MetaMathParser::handleStatement(const string& label, const string& operation, const vector<string>& arguments, const vector<string>& proof)
{
	//  Constant declaration
	if(operation == L"$c") {
		assert(label.empty() && proof.empty());
		assert(currentScope() == _globalScope);
		for(const string& symbol: arguments)
			currentScope()->addConstant(symbol);
	
	// Variable declaration
	} else if(operation == L"$v") {
		assert(label.empty() && proof.empty());
		for(const string& symbol: arguments)
			currentScope()->addVariable(symbol);
	
	// Disjoint variable requirement
	} else if(operation == L"$d") {
		assert(label.empty() && proof.empty());
		assert(arguments.size() >= 2);
		currentScope()->requireDistinct(arguments);
	
	// Floating (variable-type) hypothesis
	} else if(operation == L"$f") {
		assert(proof.empty());
		assert(!label.empty());
		assert(arguments.size() == 2);
		currentScope()->floatingHypothesis(label, arguments[0], arguments[1]);
		
	} else if(operation == L"$e") {
		assert(proof.empty());
		assert(!label.empty());
		assert(arguments.size() >= 2);
		currentScope()->essentialHypothesis(label, arguments);
		
	} else if(operation == L"$a") {
		assert(proof.empty());
		assert(!label.empty());
		assert(arguments.size() >= 2);
		currentScope()->axiom(label, arguments);
		
	} else if(operation == L"$p") {
		assert(!proof.empty());
		assert(!label.empty());
		assert(arguments.size() >= 2);
		currentScope()->derived(label, arguments, proof);
		
	}
}
string MetaMathParser::improveToken(const string& token)
{
	if(token == L"->") return L"→";
	if(token == L"-.") return L"¬";
	if(token == L"|-") return L"⊢";
	if(token == L"ps") return L"ψ";
	if(token == L"ph") return L"φ";
	if(token == L"ch") return L"χ";
	if(token == L"th") return L"θ";
	if(token == L"ta") return L"τ";
	if(token == L"<->") return L"↔";
	if(token == L"\\/") return L"∨";
	if(token == L"/\\") return L"∧";
	if(token == L"A.") return L"∀";
	if(token == L"E.") return L"∃";
	if(token == L"E!") return L"∃!";
	if(token == L"=/=") return L"≠";
	if(token == L"e.") return L"∊";
	if(token == L"C_") return L"⊆";
	if(token == L"C.") return L"⊂";
	if(token == L"<.") return L"⟨";
	if(token == L">.") return L"⟩";
	if(token == L"...") return L"…";
	if(token == L"-->") return L"—→";
	if(token == L"~~>") return L"⇝";
	if(token == L"~~>m") return L"⇝m";
	if(token == L"~~>v") return L"⇝v";
	if(token == L"~~>t") return L"⇝t";
	if(token == L"ZZ") return L"ℤ";
	if(token == L"ZZ>=") return L"ℤ>=";
	if(token == L"NN") return L"ℕ";
	if(token == L"NN0") return L"ℕ⁰";
	if(token == L"QQ") return L"ℚ";
	if(token == L"RR") return L"ℝ";
	if(token == L"RR+") return L"ℝ⁺";
	if(token == L"RR*") return L"ℝ*";
	if(token == L"CC") return L"ℂ";
	if(token == L"i^i") return L"∩";
	if(token == L"|^|") return L"⋂";
	if(token == L"u.") return L"∪";
	if(token == L"U.") return L"⋃";
	if(token == L">_") return L"≥";
	if(token == L"<_") return L"≤";
	if(token == L"x.") return L"·";
	if(token == L"sum_") return L"Σ";
	if(token == L"~~") return L"≈";
	if(token == L"o.") return L"◦";
	if(token == L"^m") return L"↑m";
	if(token == L"^pm") return L"↑pm";
	if(token == L"|->") return L"↦";
	return token;
}

