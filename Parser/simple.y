%name Simple
%include {#include "Parser/simple_Lexer.hxx"}
%include {#include "IR/SymbolVertex.h"}
%include {#include "fixups.h"}
%token_type {simple::Token*}

start ::= statements TokenTERMINATION.
statements ::= statements statement.
statements ::= .
statement ::= TokenEndOfStatement.
statement ::= call TokenEndOfStatement.
statement ::= closure TokenEndOfStatement.


call ::= symbols(returns) TokenAssignment symbol(function) symbols(arguments).
{
	wcerr << returns << function << arguments;
}

closure ::= symbol(function) symbols(arguments) TokenClosure symbols(returns).
{
	wcerr << returns << function << arguments;
}


%type symbols { vector<SymbolVertex*>* }
%destructor symbols { delete $$; }

symbols(R) ::= symbols(A) symbol(B).
{
	R = A;
	A.push_back(B);
}

symbols(R) ::= .
{
	R = new vector<SymbolVertex*>();
}


%type symbol { SymbolVertex* }
// No destructor since we intend to keep these.

symbol(R) ::= TokenIdentifier(tokenId) .
{
	SymbolVertex* R = 0;
	string id = tokenId->pretty_wchar_text();
	wcerr << "Todo: lookup identifier " << id << endl;
}

additional_tokens ::= TokenUNINITIALIZED TokenFAILURE
