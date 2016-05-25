#pragma once
#include <string>
#include <iostream>

class Parser
{
public:
	Parser();
	Parser& parseFile(const std::wstring& filename);
	Parser& parse(std::wistream& stream);
	Parser& parseString(const std::wstring& contents);
	
private:
	std::wstring _filename;
	
	// Syntax
	static constexpr wchar_t identifier_separator = L' ';
	static constexpr wchar_t indentation = L'\t';
	static constexpr wchar_t statement_separator = L'\n';
	static constexpr wchar_t substatement_begin = L'(';
	static constexpr wchar_t substatement_end = L')';
	static constexpr wchar_t quote_begin = L'“';
	static constexpr wchar_t quote_end = L'”';
	
	// Tokens
	void begin_scope();
	void end_scope();
	void begin_statement();
	void end_statement();
	void begin_substatement();
	void end_substatement();
	void identifier(const std::wstring& content);
	void quote(const std::wstring& content);
};
