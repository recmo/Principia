#include "Parser/simple.h"
#include "Parser/simple.qx.h"
#include "Parser/simple.y.h"

void* SimpleAlloc(void *(*mallocProc)(size_t));
void SimpleFree(void*, void (*freeProc)(void*));
void Simple(void*, int, quex::Token*, IntRep* ir);

IntRep* Parse(string filename, IntRep* ir)
{
	/// TODO: Scoping:
	///   Maintain a stack of scopes (identifier → SymbolVertex pairs)
	///     arg-symbol → look in whole stack, otherwise add to current scope as undefined
	///     ret-symbol → look in currect scope, if undefined then define, otherwise override or create
	/// TODO: Error handling
	if(ir == 0) ir = new IntRep();
	quex::simple lexer(encodeLocal(filename));
	quex::Token* token = 0;
	
	void* parser = SimpleAlloc(malloc);
	do {
		lexer.receive(&token);
		wcerr << "Passing token: " <<  decodeLocal(token->type_id_name()) << " " << token->pretty_wchar_text() << endl;
		Simple(parser, token->type_id(), token, ir);
	} while (token->type_id() != TokenTERMINATION);
	SimpleFree(parser, free);
	return ir;
}


