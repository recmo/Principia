#include <fixups.h>
#include "Parser/simple.qx.h"
#include "Parser/simple.y.h"
#include <valgrind/callgrind.h>

/*
void* SimpleAlloc(void *(*mallocProc)(size_t));
void SimpleFree(void*, void (*freeProc)(void*));
void Simple(void*, int, simple::Token*);

void Parse(string filename)
{
	CALLGRIND_START_INSTRUMENTATION;
	
	Lexer lexer(encodeLocal(filename));
	Token* token = 0;
	void* parser = SimpleAlloc(malloc);
	do {
		lexer.receive(&token);
		Simple(parser, token->type_id(), token);
	} while (token->type_id() != TokenTERMINATION);
	SimpleFree(parser, free);

	CALLGRIND_STOP_INSTRUMENTATION;
}
*/

