#include "Parser.h"
#include <Unicode/convert.h>
#include <Unicode/exceptions.h>
#include <vector>
#include <sstream>
#include <fstream>

Parser::Parser()
{
}

Parser& Parser::parseString(const std::wstring& contents)
{
	if(_filename.empty())
		_filename = L"<string>";
	std::wstringstream stream(contents);
	return parse(stream);
}

Parser& Parser::parseFile(const string& filename)
{
	_filename = filename;
	std::wifstream stream(encodeLocal(_filename));
	if(!stream.is_open())
		throw runtime_error(L"Could not open file.");
	return parse(stream);
}

Parser& Parser::parse(std::wistream& stream)
{
	if(_filename.empty())
		_filename = L"<stream>";
	using std::wcerr;
	uint indentation_depth = 0;
	uint statement_depth = 0;
	
	std::wstring s = L" ";
	while(stream.good()) {
		wchar_t c = stream.get();
		
		// Statements separator and indentation
		if(c == statement_separator) {
			// All sub statements must be closed
			if(statement_depth > 1) {
				throw runtime_error(L"Missing )");
			} else if(statement_depth == 1) {
				end_statement();
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
				begin_scope();
				++indentation_depth;
			} else if(new_identation_depth == indentation_depth) {
				// Same scope
			} else {
				while(new_identation_depth < indentation_depth) {
					end_scope();
					--indentation_depth;
				}
			}
			begin_statement();
			++statement_depth;
		} else if(c == indentation) {
			throw runtime_error(L"Can not have indentation here.");
		
		} else if(c == identifier_separator) {
			// Nothing
		
		// Substatements
		} else if(c == substatement_begin) {
			begin_substatement();
			++statement_depth;
		} else if(c == substatement_end) {
			if(statement_depth <= 1) {
				throw runtime_error(L"Unmatched ) found.");
			}
			end_substatement();
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
			quote(content);
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
			identifier(content);
		}
	}
	if(statement_depth == 1)
		end_statement();
	while(indentation_depth--)
		end_scope();
	
	return *this;
}

void Parser::begin_scope()
{
	std::wcerr << "BEGIN SCOPE\n";
}

void Parser::end_scope()
{
	std::wcerr << "END SCOPE\n";
}

void Parser::begin_statement()
{
	std::wcerr << "BEGIN STATEMENT\n";
}

void Parser::end_statement()
{
	std::wcerr << "END STATEMENT\n";
}

void Parser::begin_substatement()
{
	std::wcerr << "BEGIN SUBSTATEMENT\n";
}

void Parser::end_substatement()
{
	std::wcerr << "END SUBSTATEMENT\n";
}

void Parser::identifier(const std::wstring& content)
{
	std::wcerr << "IDENTFIER " << content << "\n";
}

void Parser::quote(const std::wstring& content)
{
	std::wcerr << "QUOTE " << content << "\n";
}

