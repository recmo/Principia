%name Grammar
%include {
	#include "fixups.h"
	#include "IR/SymbolVertex.h"
	#include "Parser/Parser.qx.h"
	#include "Parser/Parser.h"
}
%token_type {quex::Token*}
%extra_argument {Parser* p}
%left TokenFAILURE TokenUNINITIALIZED.
%left TokenINDENT TokenDEDENT TokenNODENT.

// Define a list of statements
start ::= statements TokenTERMINATION.
statements ::= statements call statementSeparator.
statements ::= statements closure statementSeparator.
statements ::= statements closure(C) TokenBlockBegin. { p->pushScope(C); }
statements ::= statements error. { wcerr << "Parser error in statement." << endl; }
statements ::= .

// Different statement separators
statementSeparator ::= TokenEndOfStatement.
statementSeparator ::= TokenBlockEnd. { p->popScope(); }

// 
//  C a l l   s t a t e m e n t :
// 

%type call { CallNode* }
call(R) ::= symbols(retList) TokenAssignment expressions(argList).
{
	assert(argList->size() >= 1);
	SymbolVertex* function = argList->at(0);
	argList->erase(argList->begin());
	R = p->createCall(function, *retList, *argList);
	delete retList;
	delete argList;
}

// 
//  C l o s u r e   s t a t e m e n t :
// 

%type closure { ClosureNode* }
closure(R) ::= symbols(argList) TokenClosure expressions(retList).
{
	assert(argList->size() >= 1);
	SymbolVertex* function = argList->at(0);
	argList->erase(argList->begin());
	R = p->createClosure(function, *retList, *argList);
	delete retList;
	delete argList;
}

// 
//  S y m b o l s   l i s t :
// 

%type symbols {vector<SymbolVertex*>*}
%destructor symbols {
	foreach(SymbolVertex* symbol, *$$)
		delete symbol;
	delete $$;
}
symbols(outList) ::= symbols(inList) TokenIdentifier(inSymbol). {
	string id = inSymbol->pretty_wchar_text();
	SymbolVertex* symbol = p->declareId(id);
	outList = inList;
	outList->push_back(symbol);
}
symbols(S) ::= .{
	S = new vector<SymbolVertex*>();
}


// 
// E x p r e s s i o n   l i s t :
// 

%type expressions {vector<SymbolVertex*>*}
%destructor expressions {
	foreach(SymbolVertex* symbol, *$$)
		delete symbol;
	delete $$;
}
expressions(outList) ::= expressions(inList) expression(inSymbol). {
	outList = inList;
	outList->push_back(inSymbol);
}
expressions(S) ::= . {
	S = new vector<SymbolVertex*>();
}

// 
//  E x p r e s s i o n :
// 

%type expression {SymbolVertex*}
%destructor expression { delete $$; }
expression(R) ::= TokenIdentifier(S). { R = p->resolveId(S->pretty_wchar_text()); }
expression(R) ::= TokenNumber(N).     { R = p->parseNumber(N->pretty_wchar_text()); }
expression(R) ::= TokenQuotation(Q).  { R = p->stringLitteral(Q->pretty_wchar_text()); }

// In place call
expression(R) ::= TokenBracketOpen symbols(out) TokenAssignment expressions(in) TokenBracketClose. 
{
	assert(in->size() >= 1);
	R = p->declareAnonymous();
	out->insert(out->begin(), R);
	SymbolVertex* function = in->at(0);
	in->erase(in->begin());
	p->createCall(function, *out, *in);
	delete in;
	delete out;
}

// Anonymous closure
expression(R) ::= TokenBracketOpen symbols(out) TokenClosure expressions(in) TokenBracketClose.
{
	R = p->declareAnonymous();
	p->createClosure(R, *in, *out);
	delete in;
	delete out;
}

// 
//  E v e n t   h a n d l i n g :
// 

%parse_accept {
	wcerr << "parsing complete!" << endl;
}

%syntax_error {
	wcerr << "Syntax error. Continuing..." << endl;
	// throw "No we don't";
}

%parse_failure {
	wcerr << "Giving up.  Parser is hopelessly lost..." << endl;
}

%stack_overflow {
		wcerr << "Giving up.  Parser stack overflow" << endl;
}

