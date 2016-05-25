#include "Lexer.h"
#include <Unicode/convert.h>
#include <Unicode/exceptions.h>
#include <vector>
#include <sstream>
#include <fstream>

Lexer::Lexer()
: Lexer(std::make_shared<DebugConsumer>())
{
}

Lexer::Lexer(std::shared_ptr< Lexer::TokenConsumer > consumer)
: _consumer(consumer)
, _filename()
{
	assert(_consumer != nullptr);
}

Lexer& Lexer::parseString(const std::wstring& contents)
{
	if(_filename.empty())
		_filename = L"<string>";
	std::wstringstream stream(contents);
	return parse(stream);
}

Lexer& Lexer::parseFile(const string& filename)
{
	_filename = filename;
	std::wifstream stream(encodeLocal(_filename));
	if(!stream.is_open())
		throw runtime_error(L"Could not open file.");
	return parse(stream);
}

Lexer& Lexer::parse(std::wistream& stream)
{
	if(_filename.empty())
		_filename = L"<stream>";
	using std::wcerr;
	uint indentation_depth = 0;
	uint statement_depth = 0;
	
	_consumer->begin_scope();
	
	std::wstring s = L" ";
	while(stream.good()) {
		wchar_t c = stream.get();
		
		// Statements separator and indentation
		if(c == statement_separator) {
			// All sub statements must be closed
			if(statement_depth > 1) {
				throw runtime_error(L"Missing )");
			} else if(statement_depth == 1) {
				_consumer->end_statement();
				statement_depth = 0;
			}
			
			// Indentation mode:
			uint new_identation_depth = 0;
			while(stream.peek() == indentation) {
				++new_identation_depth;
				stream.get();
			}
			// wcerr << "INDENTATION " << new_identation_depth << "\n";
			if(new_identation_depth > indentation_depth + 1) {
				throw runtime_error(L"Can not open more than one scope at a time.");
			} else if(new_identation_depth == indentation_depth + 1) {
				_consumer->begin_scope();
				++indentation_depth;
			} else if(new_identation_depth == indentation_depth) {
				// Same scope
			} else {
				while(new_identation_depth < indentation_depth) {
					_consumer->end_scope();
					--indentation_depth;
				}
			}
			_consumer->begin_statement();
			++statement_depth;
		} else if(c == indentation) {
			throw runtime_error(L"Can not have indentation here.");
		
		} else if(c == identifier_separator) {
			// Nothing
		
		// Substatements
		} else if(c == substatement_begin) {
			_consumer->begin_substatement();
			++statement_depth;
		} else if(c == substatement_end) {
			if(statement_depth <= 1) {
				throw runtime_error(L"Unmatched ) found.");
			}
			_consumer->end_substatement();
			--statement_depth;
		
		// Quote mode
		} else if(c == quote_begin) {
			std::wstring content;
			uint nesting = 1;
			while(true) {
				if(!stream.good())
					throw runtime_error(L"Found unmatched “.");
				c = stream.get();
				if(c == quote_begin)
					++nesting;
				if(c == quote_end)
					--nesting;
				if(nesting == 0)
					break;
				content += c;
			}
			_consumer->quote(content);
		} else if(c == quote_end) {
			throw runtime_error(L"Found unmatched ”.");
		
		// Identifier mode
		} else {
			std::wstring content;
			content += c;
			while(true) {
				if(!stream.good())
					break;
				c = stream.peek();
				if(c == statement_separator || c == identifier_separator || c == substatement_end)
					break;
				c = stream.get();
				if(c == quote_begin || c == quote_end || c == indentation)
					throw runtime_error(L"Unexpected character.");
				content += c;
			}
			_consumer->identifier(content);
		}
	}
	if(statement_depth == 1)
		_consumer->end_statement();
	while(indentation_depth--)
		_consumer->end_scope();
	
	_consumer->end_scope();
	
	return *this;
}

void Lexer::DebugConsumer::begin_scope()
{
	std::wcerr << "BEGIN SCOPE\n";
}

void Lexer::DebugConsumer::end_scope()
{
	std::wcerr << "END SCOPE\n";
}

void Lexer::DebugConsumer::begin_statement()
{
	std::wcerr << "BEGIN STATEMENT\n";
}

void Lexer::DebugConsumer::end_statement()
{
	std::wcerr << "END STATEMENT\n";
}

void Lexer::DebugConsumer::begin_substatement()
{
	std::wcerr << "BEGIN SUBSTATEMENT\n";
}

void Lexer::DebugConsumer::end_substatement()
{
	std::wcerr << "END SUBSTATEMENT\n";
}

void Lexer::DebugConsumer::identifier(const std::wstring& content)
{
	std::wcerr << "IDENTFIER " << content << "\n";
}

void Lexer::DebugConsumer::quote(const std::wstring& content)
{
	std::wcerr << "QUOTE " << content << "\n";
}

