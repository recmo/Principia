#include "Parser/Parser.h"
#include "Parser/Parser.qx.h"
#include "DFG/DataFlowGraph.h"
#include "DFG/Node.h"
#include "DFG/Edge.h"
#include "Source.h"
#include "Constant.h"
#include "Identifier.h"

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
		
		// Debug print token
		wcerr << filename << L" ";
		wcerr << _token->line_number() << L":";
		wcerr << _token->column_number() << L" ";
		wcerr << decodeLocal(_token->type_id_name());
		if(_token->type_id() == TokenIdentifier)
			wcerr << L"\t"<<  _token->pretty_wchar_text();
		wcerr << endl;
		
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

Source Parser::source(bool hasLexeme)
{
	int fromLine = _token->line_number();
	int fromColumn = _token->column_number();
	int toLine = fromLine;
	int toColumn = fromColumn;
	if(hasLexeme) toColumn += lexeme().size() - 1;
	return Source(_filename, fromLine, fromColumn, toLine, toColumn);
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
	wcerr << "Searching for " << id << endl;
	// Search scope
	for(auto scopei = _scopeStack.rbegin(); scopei != _scopeStack.rend() && edge == 0; ++scopei) {
		Scope& scope = *scopei;
		for(auto symboli = scope.begin(); symboli != scope.end() && edge == 0; ++symboli) {
			wcerr << (*symboli).first  << endl;
			if((*symboli).first == id) {
				if(_expressionStack.back().type != undetermined || (*symboli).second->source() == 0)
					edge = (*symboli).second;
			}
		}
	}
	wcerr << "Found : " << ((edge == 0) ? "No" : "Yes") << endl;
	if(edge == 0) {
		wcerr << "NEW Symbol " << id << endl;
		// Declare new symbol
		edge = new Edge();
		edge->set(Identifier(id));
		edge->set(source());
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
	Source location(_filename, fromLine, fromColumn, toLine, toColumn);
	
	Edge* constantEdge = new Edge;
	constantEdge->set(location);
	constantEdge->set(Constant(contents));
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
		if(basePos < litteral.size())
		{
			// Read the exponent sign if its there
			int expSign = 1;
			if(litteral[basePos] == exponentPositive) basePos++;
			else if(litteral[basePos] == exponentNegative) {
				expSign = -1;
				basePos++;
			}
			
			// Read the rest of the exponent
			exponent = 0;
			for(; basePos < litteral.size(); ++basePos)
			{
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
	if(seenPoint) exponent += separators;
	
	Edge* constantEdge = new Edge;
	constantEdge->set(source());
	constantEdge->set(Constant(mantissa * pow(base, exponent)));
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
	Source start = group->get<Source>();
	Source end = source(false);
	Source source(_filename, start.fromLine(), start.fromColumn(), end.toLine(), end.toColumn());
	group->set(source);
	_expressionStack[i - 1].in.push_back(group);
	_expressionStack.pop_back();
}

void Parser::parseFailure()
{
	Source s = source();
	wcerr << endl << s << L": Syntax error." << endl;
	s.printCaret(wcerr);
	throw L"Syntax error.";
}

Edge* Parser::finishNode()
{
	assert(_expressionStack.back().type != undetermined);
	NodeType type = (_expressionStack.back().type == call) ? NodeType::Call : NodeType::Closure;
	Node* node = new Node(type,_expressionStack.back().in.size(), _expressionStack.back().out.size());
	for(int i = 0; i != _expressionStack.back().out.size(); ++i)
	{
		Edge* from = _expressionStack.back().out[i];
		Edge* to = node->out(i);
		wcerr << from << L" ↦ " << to << endl;
		
		// Replace in stack
		_expressionStack.back().out[i] = to;
		
		// Copy info
		if(from->has<Identifier>() && !to->has<Identifier>())
			to->set(from->get<Identifier>());
		if(from->has<Source>() && !to->has<Source>())
			to->set(from->get<Source>());
		
		// Replace in DFG
		from->replaceWith(to);
		
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
	for(int i = 0; i != _expressionStack.back().in.size(); ++i)
		node->connect(i, _expressionStack.back().in[i]);
	_dfg->nodes().push_back(node);
	_expressionStack.back().type = undetermined;
	_expressionStack.back().out.clear();
	_expressionStack.back().in.clear();
	return (node->outArrity() > 0) ? node->out(0) : 0;
}

