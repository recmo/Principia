#include "fixups.h"
#include "Parser/Parser.h"
#include "IR/IntRep.h"
#include "IR/SymbolVertex.h"
#include "Interpreter/Interpreter.h"
#include "Interpreter/Value.h"
#include <fstream>
#include "Passes/Validator.h"
#include "IR/CallNode.h"
#include "IR/ClosureNode.h"
#include "Parser/Parser.h"

/*

TODO: Scoping

TODO: Number litterals

TODO: In place syntax, dropping the first out symbol

  (r₂ r₃ ≔ f a₁ a₂)
  
A simplification may be used when there is only one return symbol:
  
  (≔ f a₁ a₂)

Similarly anonymous λ-functions:

  (a₁ a₂ ↦ r₁ r₂)

Now if the 'if' function looks at its first argument and returns
either the second or third then this will create a lazy if:

result ≔ (≔ if condition (↦ lazy_then) (↦ lazy_else))

Beautiful!





TODO: Allow syntax modifications in language:

Lexer -> Preprocessor -> Parser
Lexer: Chunk source into identifiers
Preprocessor: resolve scoping and references
Parser: 


[parse rule:  #1 + #2 ↦ (. ≔ plus #1 #2) ]
[parse rule:  if #1 then #2 else #3  ↦ ( . ≔  if(#1, λ(#2), λ(#3)() ) ]
[parse rule:  /#1/ ↦ ( . ≔  regexp_parse("#3") ) ]
etc…

*/


sint32 Main(const vector<string>& args)
{
	/// TODO: Simplify IR, make it HOAS + De Bruijn indices
	/// Definition sites:
	///  0) external (constant, builtin, etc…)
	///  1) return values from calls
	///  2) closure function
	///  3) closure argument
	/// Externaly store metadata such as identifiers
	
	/// TODO: Re-use contexts
	/// TODO: Garbage management of closures
	
	/// TODO: Validate programs
	///  Rule: Only cycles allowed are in ClosureDefinition from arguments or closure to returns
	/// TODO: Identifier prefixes
	/// TODO: Calculate minimal closures
	/// TODO: Calculate minimal stacks
	/// TODO: A call should never require memory allocation
	/// TODO: Correct memory management in interpreter
	/// TODO: Store minimal context in closures
	/// TODO: Fixed stacks per closure
	
	/// TODO: String support
	/// TODO: Standard library with I/O
	/// TODO: Reflection
	/// TODO: Dynamic syntax sugar system
	/// TODO: Self-interpreter
	/// TODO: Memory management in language
	/// TODO: Inline hot closures and compile to machine code
	
	/// TODO: Expand expressions between ( ) as infix math using temps
	
	wcerr << L"Simple C++ interpreter for the language using ≔ and ↦." << endl;
	wcerr << endl;
	if(args.size() < 3)
	{
		wcerr << "Usage: proglang source_file function [arguments]*" << endl;
		throw std::runtime_error("Not enough arguments.");
	}
	
	// Parse file
	wcerr << L"Parsing file…" << flush;
	Parser parser;
	parser.parse(args[1]);
	IntRep* ir = parser.ir();
	wcerr << endl;
	
	wcerr << endl;
	foreach(SymbolVertex* symbol, ir->symbols())
		wcerr << L"Symbol " << symbol << L" " << symbol->definitionType().toString() << endl;
	wcerr << endl;
	
	// Validate IR
	wcerr << L"Validating…" << flush;
	wcerr << endl << endl;
	Validator validator(ir);
	validator.validate();
	wcerr << endl;
	
	// Find function to call
	SymbolVertex* functionSymbol = 0;
	foreach(SymbolVertex* symbol, ir->symbols())
		if(symbol->identifier() == args[2])
			functionSymbol = symbol;
	if(!functionSymbol)
		throw std::runtime_error("Could not find specified function.");
	
	// Parse arguments
	vector<Value> arguments;
	for(unsigned int i = 3; i < args.size(); i++)
	{
		sint64 integer;
		double real;
		if(parse<sint64>(args[i], integer))
			arguments.push_back(integer);
		else if(parse<double>(args[i], real))
			arguments.push_back(real);
		else
			throw "Could not parse argument";
	}
	
	// Execute IR
	wcerr << L"Executing…" << flush;
	Interpreter interpreter(ir);
	vector<Value> returns;
	returns = interpreter.evaluateFunction(functionSymbol, arguments);
	wcerr << endl;
	
	wcout << returns << endl;
	return 0;
}
