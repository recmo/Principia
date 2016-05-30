#pragma once
#include <string>
#include <iostream>
#include <memory>
#include <vector>

namespace Parser {

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
	std::weak_ptr<Node> binding_site;
	bool is_binding_site = false;
};

// Lexer
Node lexer(std::wistream& stream);

// Binder
void deanonymize(Node& module);
void tag_binding_sites(Node& module);
void undebruijn(Node& module);
void bind(Node& module);

// Parser
Node parseFile(const std::wstring& filename);
Node parseString(const std::wstring& contents);
Node paserStream(std::wistream& stream);

};
