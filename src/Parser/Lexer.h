#pragma once
#include <string>
#include <iostream>
#include <memory>

class Lexer
{
public:
	class TokenConsumer;
	class DebugConsumer;
	
	Lexer();
	Lexer(std::shared_ptr<TokenConsumer> consumer);
	Lexer& parseFile(const std::wstring& filename);
	Lexer& parse(std::wistream& stream);
	Lexer& parseString(const std::wstring& contents);
	
private:
	std::shared_ptr<TokenConsumer> _consumer;
	std::wstring _filename;
	
	// Syntax
	static constexpr wchar_t identifier_separator = L' ';
	static constexpr wchar_t indentation          = L'\t';
	static constexpr wchar_t statement_separator  = L'\n';
	static constexpr wchar_t substatement_begin   = L'(';
	static constexpr wchar_t substatement_end     = L')';
	static constexpr wchar_t quote_begin          = L'“';
	static constexpr wchar_t quote_end            = L'”';
};

class Lexer::TokenConsumer {
public:
	virtual void begin_scope() = 0;
	virtual void end_scope() = 0;
	virtual void begin_statement() = 0;
	virtual void end_statement() = 0;
	virtual void begin_substatement() = 0;
	virtual void end_substatement() = 0;
	virtual void identifier(const std::wstring& content) = 0;
	virtual void quote(const std::wstring& content) = 0;
};

class Lexer::DebugConsumer: public TokenConsumer {
public:
	virtual void begin_scope();
	virtual void end_scope();
	virtual void begin_statement();
	virtual void end_statement();
	virtual void begin_substatement();
	virtual void end_substatement();
	virtual void identifier(const std::wstring& content);
	virtual void quote(const std::wstring& content);
};
