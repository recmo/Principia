#include "Parser/Parser2.h"
#include "Parser/Parser.qx.h"
#include "SourceProperty.h"
#include "ConstantProperty.h"
#include "IdentifierProperty.h"

#define debug true

Parser2::Parser2()
: _token(0)
, _unread(false)
{
}

Parser2& Parser2::parse(const string& filename)
{
	_filename = filename;
	_parser = new quex::Parser(encodeLocal(_filename));
	readNext();
	_tree = parseFile();
	delete _parser;
	return *this;
}

void Parser2::readNext()
{
	if(_unread) {
		_unread = false;
		return;
	}
	_parser->receive(&_token);
	
	if(debug && false) {
		// Debug print token
		wcerr << endl;
		wcerr << "TOKEN: ";
		wcerr << decodeLocal(_token->type_id_name());
		wcerr << " at ";
		wcerr << _token->line_number() << L":";
		wcerr << _token->column_number();
		if(_token->type_id() == TokenIdentifier)
			wcerr << L"\t"<<  _token->pretty_wchar_text();
		wcerr << endl;
	}
	wcerr << decodeLocal(_token->type_id_name()) << " ";
}

void Parser2::unread()
{
	assert(_unread == false);
	_unread = true;
}

uint32 Parser2::token()
{
	return _token->type_id();
}

string Parser2::lexeme()
{
	return decodeUtf8(reinterpret_cast<const char*>(_token->get_text().c_str()));
}

SourceProperty Parser2::source(bool hasLexeme)
{
	int fromLine = _token->line_number();
	int fromColumn = _token->column_number();
	int toLine = fromLine;
	int toColumn = fromColumn;
	if(hasLexeme) toColumn += lexeme().size() - 1;
	return SourceProperty(_filename, fromLine, fromColumn, toLine, toColumn);
}

void Parser2::parseFailure(string location)
{
	SourceProperty s = source();
	if(location.empty())
		wcerr << endl << s << L": Syntax error." << endl;
	else
		wcerr << endl << s << L": Syntax error in " << location << "." << endl;
	s.printCaret(wcerr);
	throw L"Syntax error.";
}

ParseTree* Parser2::parseFile()
{
	ParseTree* tree = new ParseTree;
	for(;;readNext()) switch(token()) {
	case TokenStatementSeparator: continue;
	case TokenBlockBegin: tree->topLevel()->add(parseScope()); continue;
	case TokenIdentifier: tree->topLevel()->add(parseStatement()); continue;
	case TokenEndOfStream: return tree;
	default: parseFailure(L"file"); delete tree; return 0;
	}
}

ParseTree::Scope* Parser2::parseScope()
{
	// Eat the block begin
	assert(token() == TokenBlockBegin);
	readNext();
	
	// Parse the scope
	ParseTree::Scope* scope = new ParseTree::Scope;
	for(;;readNext()) switch(token()) {
	case TokenStatementSeparator: continue;
	case TokenBlockBegin: scope->add(parseScope()); continue;
	case TokenIdentifier: scope->add(parseStatement()); continue;
	case TokenBlockEnd: return scope;
	default: parseFailure(L"scope"); delete scope; return 0;
	}
}

ParseTree::Statement* Parser2::parseStatement()
{
	ParseTree::Statement* statement = new ParseTree::Statement;
	
	// Parse the out identifiers
	while(token() == TokenIdentifier) {
		statement->addOut(parseIdentifier());
		readNext();
	}
	
	// Parse the statement type
	switch(token()) {
	case TokenAssignment: statement->type(NodeType::Call); break;
	case TokenClosure: statement->type(NodeType::Closure); break;
	default: parseFailure(L"statement type"); delete statement; return 0;
	}
	readNext();
	
	// Parse the in expressions
	for(;;readNext()) switch(token()) {
	case TokenIdentifier: statement->addIn(parseIdentifier()); continue;
	case TokenBracketOpen: statement->addIn(parseInlineStatement()); continue;
	case TokenNumber: statement->addIn(parseNumber()); continue;
	case TokenQuotation: statement->addIn(parseQuotation()); continue;
	case TokenBlockBegin:
	case TokenBlockEnd: unread();
	case TokenStatementSeparator: return statement;
	default: parseFailure(L"statement inputs"); delete statement; return 0;
	}
}

ParseTree::InlineStatement* Parser2::parseInlineStatement()
{
	ParseTree::InlineStatement* statement = new ParseTree::InlineStatement;
	
	// Eat the bracker open
	assert(token() == TokenBracketOpen);
	readNext();
	
	// Parse the out identifiers
	while(token() == TokenIdentifier || token() == TokenBracketValue) {
		// parseIdentifier will work on TokenBracketValue as well
		statement->addOut(parseIdentifier());
		readNext();
	}
	
	// Parse the statement type
	switch(token()) {
	case TokenAssignment: statement->type(NodeType::Call); break;
	case TokenClosure: statement->type(NodeType::Closure); break;
	default: parseFailure(L"inline statement type"); delete statement; return 0;
	}
	readNext();
	
	// Parse the in expressions
	for(;;readNext()) switch(token()) {
	case TokenIdentifier: statement->addIn(parseIdentifier()); continue;
	case TokenBracketOpen: statement->addIn(parseInlineStatement()); continue;
	case TokenNumber: statement->addIn(parseNumber()); continue;
	case TokenQuotation: statement->addIn(parseQuotation()); continue;
	case TokenBracketClose: return statement;
	default: parseFailure(L"inline statement inputs"); delete statement; return 0;
	}
}

ParseTree::Identifier* Parser2::parseIdentifier()
{
	ParseTree::Identifier* id = new ParseTree::Identifier;
	id->name(lexeme());
	id->soureProperty(source(true));
	wcerr << id->soureProperty() << endl;
	return id;
}

ParseTree::Constant* Parser2::parseNumber()
{
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
		return new ParseTree::Constant(value);
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
		return new ParseTree::Constant(value);
	}
}

ParseTree::Constant* Parser2::parseQuotation()
{
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
	
	return new ParseTree::Constant(contents);
}

