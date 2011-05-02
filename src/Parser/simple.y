%name Simple
%include {#include "fixups.h"}
%left TokenFAILURE TokenUNINITIALIZED.

start ::= statements TokenTERMINATION.
statements ::= statements statement.
statements ::= .
statement ::= TokenEndOfStatement.
statement ::= call TokenEndOfStatement.
statement ::= closure TokenEndOfStatement.

call ::= symbols TokenAssignment symbols.
closure ::= symbols TokenClosure symbols.

symbols ::= symbols symbol.
symbols ::= .

symbol ::= TokenIdentifier.




