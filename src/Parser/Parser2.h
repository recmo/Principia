#pragma once
#include <fixups.h>
#include <Parser/SourceProperty.h>
#include "ParseTree.h"
namespace quex { class Parser; class Token; }

class Parser2
{
public:
	Parser2();
	
	Parser2& parse(const string& filename);
	
	ParseTree* tree() { return _tree; }
	
private:
	typedef quex::Token Token;
	string _filename;
	quex::Parser* _parser;
	quex::Token* _token;
	ParseTree* _tree;
	ParseTree::Statement* _statement;
	bool _unread;
	
	void readNext();
	void unread();
	uint32 token();
	string lexeme();
	SourceProperty source(bool hasLexeme = true);
	
	ParseTree* parseFile();
	ParseTree::Statement* parseStatement();
	ParseTree::InlineStatement* parseInlineStatement();
	ParseTree::Scope* parseScope();
	ParseTree::Identifier* parseIdentifier();
	ParseTree::Constant* parseNumber();
	ParseTree::Constant* parseQuotation();
	
	void parseFailure(string location = string());
};
