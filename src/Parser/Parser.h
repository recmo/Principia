#pragma once
#include <string>
#include <iostream>
#include <memory>
#include <vector>

namespace Parser {

typedef std::pair<uint, uint> Symbol;

// Syntax
static constexpr wchar_t identifier_separator = L' ';
static constexpr wchar_t indentation          = L'\t';
static constexpr wchar_t statement_separator  = L'\n';
static constexpr wchar_t substatement_begin   = L'(';
static constexpr wchar_t substatement_end     = L')';
static constexpr wchar_t quote_begin          = L'“';
static constexpr wchar_t quote_end            = L'”';

// Keywords
static constexpr wchar_t closure              = L'λ';
static constexpr wchar_t de_bruijn_1          = L'·';
static constexpr wchar_t de_bruijn_2          = L':';

// Abstract Syntax Tree
enum Kind {
	Module,
	Scope,
	Statement,
	SubStatement,
	Identifier,
	Quote
};

struct Node {
	Node(Kind k): kind(k) { }
	Kind kind;
	std::wstring filename;
	std::wstring identifier;
	std::wstring quote;
	std::vector<std::shared_ptr<Node>> children;
	std::vector<std::shared_ptr<Node>> globals;
	std::weak_ptr<Node> binding_site;
	bool is_binding_site = false;
	bool is_closure = false;
	uint closure_index = 0;
	Symbol bind_index;
};

// Lexer
std::shared_ptr<Node> lexer(std::wistream& stream);

// Parser
void parse(std::shared_ptr<Node> module);
std::shared_ptr<Node> parseFile(const std::wstring& filename);
std::shared_ptr<Node> parseString(const std::wstring& contents);
std::shared_ptr<Node> paserStream(std::wistream& stream);

void print(std::shared_ptr<Node> module);

struct Program {
	std::vector<std::wstring> symbols_import;
	std::vector<std::wstring> symbols_export;
	std::vector<std::wstring> constants;
	std::vector<uint> closures;
	std::vector<std::vector<Symbol>> calls;
};

// Compile to dense format sans identifiers
Program compile(std::shared_ptr<Node> module);
void write(std::wostream& out, const Program& program);
Program read(std::wistream& in);

};
