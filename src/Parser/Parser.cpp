#include "Parser/Parser.h"
#include "Parser/Parser.qx.h"
#include "DFG/DataFlowGraph.h"
#include "DFG/Node.h"
#include "DFG/Edge.h"
#include "SourceProperty.h"
#include "ConstantProperty.h"
#include "IdentifierProperty.h"
#include <Interpreter/Builtins.h>

#define debug false

std::wostream& operator<<(std::wostream& out, const Parser::Expression& expression)
{
	out << "(";
	out << expression.out;
	if(expression.type == Parser::closure)
		out << " ↦ ";
	else if(expression.type == Parser::call)
		out << L" ≔ ";
	else
		out << L" ? ";
	out << expression.in;
	out << ")";
	return out;
}

Parser::Expression::Expression()
: type(Parser::undetermined)
, in()
, out()
{
}

Parser::Parser()
: _dfg(new DataFlowGraph)
, _scopeStack()
, _token(0)
, _expressionStack(1)
{
	pushScope();
}

Parser::Parser(DataFlowGraph* dfg)
: _dfg(dfg)
, _scopeStack()
, _token(0)
, _expressionStack(1)
{
	assert(dfg != null);
	pushScope();
}

Parser& Parser::parse(const string& filename)
{
	_filename = filename;
	quex::Parser lexer(encodeLocal(_filename));
	do {
		lexer.receive(&_token);
		
		if(debug) {
			wcerr << endl << endl;
			wcerr << L"Scope stack: " << _scopeStack << endl;
			wcerr << L"Expression stack: " << _expressionStack << endl;
			foreach(const Node* node, _dfg->nodes())
				wcerr << node << " " << node->out() << " " << node->in() << endl; 
			
			// Debug print token
			wcerr << filename << L" ";
			wcerr << _token->line_number() << L":";
			wcerr << _token->column_number() << L" ";
			wcerr << decodeLocal(_token->type_id_name());
			if(_token->type_id() == TokenIdentifier)
				wcerr << L"\t"<<  _token->pretty_wchar_text();
			wcerr << endl;
		}
		
		// Dispatch on token type
		switch(_token->type_id()) {
			case TokenIdentifier: parseIdentifier(); break;
			case TokenQuotation: parseQuotation(); break;
			case TokenNumber: parseNumber(); break;
			case TokenAssignment: parseCall(); break;
			case TokenClosure: parseClosure(); break;
			case TokenStatementSeparator: parseStatementSeparator(); break;
			case TokenBlockBegin: parseBlockBegin(); break;
			case TokenBlockEnd: parseBlockEnd(); break;
			case TokenBracketOpen: parseBracketOpen(); break;
			case TokenBracketClose: parseBracketClose(); break;
			case TokenFailure: parseFailure(); break;
			case TokenEndOfStream: break;
			default:
				wcerr << L"Unknown token id " <<  _token->type_id();
				wcerr << L"(" << decodeUtf8(_token->type_id_name()) << L")" << endl;
				throw L"Unknown token.";
		}
	} while (_token->type_id() != TokenEndOfStream);
	
	return *this;
}

string Parser::lexeme()
{
	return decodeUtf8(reinterpret_cast<const char*>(_token->get_text().c_str()));
}

SourceProperty Parser::source(bool hasLexeme)
{
	int fromLine = _token->line_number();
	int fromColumn = _token->column_number();
	int toLine = fromLine;
	int toColumn = fromColumn;
	if(hasLexeme) toColumn += lexeme().size() - 1;
	return SourceProperty(_filename, fromLine, fromColumn, toLine, toColumn);
}

void Parser::pushScope()
{
	Scope scope;
	_scopeStack.push_back(scope);
}

void Parser::popScope()
{
	_scopeStack.pop_back();
}

void Parser::parseIdentifier()
{
	// Scoping rules different for call and closure:
	// In a closure the out values may appear in the in expression:
	// f a b c ↦ expr(f, a, b, c)
	//
	// In a call this is illegal:
	// f a b c ≔ expr(f, a, b, c)
	//
	string id = lexeme();
	bool isDefinition = true;
	
	Edge* edge = 0;
	// Search scope
	for(auto scopei = _scopeStack.rbegin(); scopei != _scopeStack.rend() && edge == 0; ++scopei) {
		Scope& scope = *scopei;
		for(auto symboli = scope.begin(); symboli != scope.end() && edge == 0; ++symboli) {
			if((*symboli).first == id) {
				if(_expressionStack.back().type != undetermined || (*symboli).second->source() == 0)
					edge = (*symboli).second;
			}
		}
	}
	if(edge == 0) {
		// Declare new symbol
		edge = new Edge();
		edge->set(IdentifierProperty(id));
		edge->set(source());
		if(builtins.find(id) != builtins.end())
			edge->set(ConstantProperty(builtins[id]));
		_scopeStack.back()[id] = edge;
	}
	if(_expressionStack.back().type == undetermined)
		_expressionStack.back().out.push_back(edge);
	else
		_expressionStack.back().in.push_back(edge);
}

void Parser::parseQuotation()
{
	assert(_expressionStack.back().type != undetermined);
	string contents = lexeme();
	
	// Count line and column number
	int num_lines = 0;
	int toColumn = 1;
	for(int i = 0; i != contents.size(); ++i) {
		if(contents[i] == (L'\n')) {
			++num_lines;
			toColumn = 1;
		} else {
			++toColumn;
		}
	}
	
	/// TODO: Fix line and col number
	int fromLine = _token->line_number();
	int fromColumn = _token->column_number();
	int toLine = fromLine;
	fromLine -= num_lines;
	SourceProperty location(_filename, fromLine, fromColumn, toLine, toColumn);
	
	Edge* constantEdge = new Edge;
	constantEdge->set(location);
	constantEdge->set(ConstantProperty(contents));
	_expressionStack.back().in.push_back(constantEdge);
}

void Parser::parseNumber()
{
	assert(_expressionStack.back().type  != undetermined);
	const string digits = L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	const wchar_t separator = L'\u2009';
	const wchar_t radixPoint = L'.';
	const string baseDigits = L"₀₁₂₃₄₅₆₇₈₉";
	const string exponentDigits = L"⁰¹²³⁴⁵⁶⁷⁸⁹";
	const wchar_t exponentPositive = L'⁺';
	const wchar_t  exponentNegative = L'⁻';
	
	string litteral = _token->pretty_wchar_text();
	uint64 mantissa;
	int base;
	int exponent;
	
	// Find a base if we got one
	size_t baseStart = litteral.find_first_of(baseDigits);
	if(baseStart != string::npos) {
		// Read the base
		size_t basePos = baseStart;
		base = 0;
		for(;;) {
			size_t digit = baseDigits.find_first_of(litteral[basePos]);
			if(digit == string::npos) break;
			base *= 10;
			base += digit;
			basePos++;
			if(basePos == litteral.size()) break;
		}
		if(basePos < litteral.size()) {
			// Read the exponent sign if its there
			int expSign = 1;
			if(litteral[basePos] == exponentPositive) basePos++;
			else if(litteral[basePos] == exponentNegative) {
				expSign = -1;
				basePos++;
			}
			
			// Read the rest of the exponent
			exponent = 0;
			for(; basePos < litteral.size(); ++basePos) {
				size_t digit = exponentDigits.find_first_of(litteral[basePos]);
				if(digit == string::npos) throw "Syntax error in number";
				exponent *= 10;
				exponent += digit;
			}
			exponent *= expSign;
		} else
			exponent = 0;
	} else {
		baseStart = litteral.size();
		base = 10;
		exponent = 0;
	}
	
	// Find the rest of the digits
	mantissa = 0;
	int separators = 0;
	bool seenPoint = false;
	for(size_t i = 0; i < baseStart; ++i) {
		if(litteral[i] == separator) {
			++separators;
			continue;
		}
		if(litteral[i] == radixPoint) {
			assert(!seenPoint);
			seenPoint = true;
			separators = 0;
			exponent -= baseStart - i - 1;
			continue;
		}
		size_t digit = digits.find_first_of(litteral[i]);
		if(digit >= base) throw "Syntax error in number";
		mantissa *= base;
		mantissa += digit;
	}
	if(seenPoint)
		exponent += separators;
	
	// Set a real or integer constant based on the radix point
	Edge* constantEdge = new Edge;
	constantEdge->set(source());
	if(seenPoint) {
		double value = mantissa * pow(base, exponent);
		constantEdge->set(ConstantProperty(value));
	} else {
		sint64 value = mantissa;
		uint64 squares = base;
		assert(exponent >= 0);
		while(exponent) {
			if(exponent & 1)
				value *= squares;
			squares *= squares;
			exponent >>= 1;
		}
		constantEdge->set(ConstantProperty(value));
	}
	_expressionStack.back().in.push_back(constantEdge);
}

void Parser::parseCall()
{
	assert(_expressionStack.back().type == undetermined);
	_expressionStack.back().type = call;
}

void Parser::parseClosure()
{
	assert(_expressionStack.back().type == undetermined);
	_expressionStack.back().type = closure;
}

void Parser::parseBlockBegin()
{
	parseStatementSeparator();
}

void Parser::parseBlockEnd()
{
	parseStatementSeparator();
}

void Parser::parseStatementSeparator()
{
	if(_expressionStack.size() == 1 && _expressionStack.back().type == undetermined)
		return;
	finishNode();
}

void Parser::parseBracketOpen()
{
	Edge* edge = new Edge;
	edge->set(source(false));
	_expressionStack.push_back(Expression());
	_expressionStack.back().out.push_back(edge);
}

void Parser::parseBracketClose()
{
	finishNode();
	int i = _expressionStack.size() - 1;
	assert(i >= 1);
	Edge* group = _expressionStack.back().out[0];
	SourceProperty start = group->get<SourceProperty>();
	SourceProperty end = source(false);
	SourceProperty source(_filename, start.fromLine(), start.fromColumn(), end.toLine(), end.toColumn());
	group->set(source);
	_expressionStack[i - 1].in.push_back(group);
	_expressionStack.pop_back();
}

void Parser::parseFailure()
{
	SourceProperty s = source();
	wcerr << endl << s << L": Syntax error." << endl;
	s.printCaret(wcerr);
	throw L"Syntax error.";
}

Edge* Parser::finishNode()
{
	assert(_expressionStack.back().type != undetermined);
	
	// Construct the new node of the correct type and size
	NodeType type = (_expressionStack.back().type == call) ? NodeType::Call : NodeType::Closure;
	Node* node = new Node(type,_expressionStack.back().in.size(), _expressionStack.back().out.size());
	
	// Constuct the output nodes
	for(int i = 0; i != _expressionStack.back().out.size(); ++i) {
		Edge* from = _expressionStack.back().out[i];
		Edge* to = node->out(i);
		
		// Replace in in
		for(int j = 0; j < _expressionStack.back().in.size(); ++j) {
			if(_expressionStack.back().in[j] == from)
				_expressionStack.back().in[j] = to;
		}
		
		// Replace in stack
		_expressionStack.back().out[i] = to;
		
		// Copy info
		if(from->has<IdentifierProperty>() && !to->has<IdentifierProperty>())
			to->set(from->get<IdentifierProperty>());
		if(from->has<SourceProperty>() && !to->has<SourceProperty>())
			to->set(from->get<SourceProperty>());
		
		// Replace in DFG
		if(debug)
			wcerr << "Replacing " << from << " with " << to << endl;
		from->replaceWith(to);
		if(debug)
			foreach(const Node* node, _dfg->nodes())
				wcerr << node << " " << node->out() << " " << node->in() << endl; 
		
		// Replace in scope as well
		for(auto scopei = _scopeStack.rbegin(); scopei != _scopeStack.rend(); ++scopei) {
			Scope& scope = *scopei;
			for(auto symboli = scope.begin(); symboli != scope.end(); ++symboli) {
				if((*symboli).second == from) {
					scope[(*symboli).first] = to;
				}
			}
		}
	}
	
	// Add to the DFG
	_dfg->nodes().push_back(node);
	
	// Set the new node's inputs
	for(int i = 0; i != _expressionStack.back().in.size(); ++i)
		node->connect(i, _expressionStack.back().in[i]);
	
	// Clear the intem in the expression stack
	_expressionStack.back().type = undetermined;
	_expressionStack.back().out.clear();
	_expressionStack.back().in.clear();
	
	// Add an identifier for printing purposes
	if(node->type() == NodeType::Call && node->inArrity() >= 1 && node->in(0)->has<IdentifierProperty>())
		node->set(IdentifierProperty(node->in(0)->get<IdentifierProperty>().value()));
	else if(node->type() == NodeType::Closure && node->outArrity() >= 1 && node->out(0)->has<IdentifierProperty>())
		node->set(IdentifierProperty(node->out(0)->get<IdentifierProperty>().value()));
	
	if(debug)
		wcerr << node << node->in() << node->out() << endl;
	
	return (node->outArrity() > 0) ? node->out(0) : 0;
}

