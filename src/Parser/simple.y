%name Simple
%include {
	#include "fixups.h"
	#include "IR/IntRep.h"
	#include "IR/CallNode.h"
	#include "IR/ClosureNode.h"
	#include "IR/SymbolVertex.h"
	#include "Parser/simple.qx.h"
}
%token_type {quex::Token*}
%extra_argument {IntRep* ir}
%left TokenFAILURE TokenUNINITIALIZED.
%left TokenINDENT TokenDEDENT TokenNODENT.

// Define a list of statements
start ::= statements TokenTERMINATION.
statements ::= statements statement statementSeparator.
statements ::= .

// Different statement separators
statementSeparator ::= TokenEndOfStatement.
statementSeparator ::= TokenBlockBegin.
statementSeparator ::= TokenBlockEnd.

// Types of statements
statement ::= call.
statement ::= closure.
statement ::= error.
{
	// TODO: Print nice message based on stack information
	
	// Something like line, column, expected ... got ...
	wcerr << "Parser error in statement." << endl;
}

// 
//  C a l l   s t a t e m e n t :
// 

call ::= symbols(retList) TokenAssignment expressions(argList).
{
	CallNode* call = new CallNode;
	auto arg = argList->begin();
	call->function(*arg++);
	for(; arg != argList->end(); ++arg)
		call->arguments().push_back(*arg);
	for(auto ret = retList->begin(); ret != retList->end(); ++ret) {
		(*ret)->setReturnedBy(call);
		call->returns().push_back(*ret);
	}
	delete retList;
	delete argList;
}

// 
//  C l o s u r e   s t a t e m e n t :
// 

closure ::= symbols(argList) TokenClosure expressions(retList).
{
	ClosureNode* closure = new ClosureNode;
	auto arg = argList->begin();
	closure->function(*arg);
	(*arg)->setFunctionOf(closure);
	for(++arg; arg != argList->end(); ++arg) {
		closure->arguments().push_back(*arg);
		(*arg)->setArgumentOf(closure);
	}
	for(auto ret = retList->begin(); ret != retList->end(); ++ret)
		closure->returns().push_back(*ret);
	delete retList;
	delete argList;
}

// 
//  S y m b o l s   l i s t :
// 

%type symbols {vector<SymbolVertex*>*}
%destructor symbols {
	wcerr << "delete " << $$ << endl;
	foreach(SymbolVertex* symbol, *$$) {
		eraseByValue(ir->symbols(), symbol);
		delete symbol;
	}
	delete $$;
}
symbols(outList) ::= symbols(inList) symbol(inSymbol).{
	outList = inList;
	outList->push_back(inSymbol);
}
symbols(S) ::= .{
	S = new vector<SymbolVertex*>();
}


// 
// E x p r e s s i o n   l i s t :
// 

%type expressions {vector<SymbolVertex*>*}
%destructor expressions {
	wcerr << "delete " << $$ << endl;
	foreach(SymbolVertex* symbol, *$$) {
		eraseByValue(ir->symbols(), symbol);
		delete symbol;
	}
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
%destructor expression {
	eraseByValue(ir->symbols(), $$);
	delete $$;
}
expression(R) ::= quotation(Q). { R = Q; }
expression(R) ::= symbol(S). { R = S; }

// Simple in place call
/*
expression(R) ::= TokenBracketOpen expressions(E) TokenBracketClose. 
{
	R = new SymbolVertex(L"anon");
	CallNode* call = new CallNode;
	auto it = E->begin();
	call->function(*it++);
	for(; it != E->end(); ++it)
		call->arguments().push_back(*it);
	call->returns().push_back(R);
	R->setReturnedBy(call);
}
*/

// Verbose in place call
expression ::= TokenBracketOpen symbols(out) TokenAssignment expressions(in) TokenBracketClose. 
{
	/// TODO: Verify that out contains only symbols. in
}

// Anonymous closure
expression ::= TokenBracketOpen symbols(out) TokenClosure expressions(in) TokenBracketClose.
{
	/// TODO: Verify that out contains only symbols. in
}

// 
//  S y m b o l :
// 

%type symbol {SymbolVertex*}
%destructor symbol {
	eraseByValue(ir->symbols(), $$);
	delete $$;
}
symbol(S) ::= TokenIdentifier(tknId). {
	// wcerr << tknId->line_number() << " " << tknId->column_number() << " " << tknId->pretty_wchar_text() << endl;
	S = 0;
	string id = tknId->pretty_wchar_text();
	foreach(SymbolVertex* symbol, ir->symbols()) {
		if(symbol->identifier() == id) {
			S = symbol;
			break;
		}
	}
	if(S == 0) {
		S = new SymbolVertex(id);
		ir->symbols().push_back(S);
	}
}

// 
//  Q u o t a t i o n :
// 

%type quotation {SymbolVertex*}
%destructor quotation {
	eraseByValue(ir->symbols(), $$);
	delete $$;
}
quotation(Q) ::= TokenQuotation(quotation). {
	Q = new SymbolVertex(L"asd");
	/// TODO: Implement!
	wcerr << L"Received quote “" << quotation->pretty_wchar_text() << L"”, ignoring for now." << endl;
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

