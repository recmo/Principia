#include "fixups.h"
#include "Parser/Parser.h"
#include "Interpreter/Interpreter.h"
#include "Interpreter/Value.h"
#include "Interpreter/Closure.h"
#include <fstream>
#include "Passes/Validator.h"
#include "Parser/Parser.h"
#include "DFG/DataFlowGraph.h"
#include "Parser/IdentifierProperty.h"
#include "Passes/DotFileWriter.h"
#include "Passes/LambdaLifter.h"
#include "Passes/ConstantClosure.h"
#include "Passes/TopologicalSorter.h"
#include <cmath>

// Boolean edges as truth values
// Theorems as 

// Implement boolean values as:
// True a b ↦ a
// False a b ↦ b
// Then the following are equivalent:
// a ≔ if condition then else
// a ≔ condition then else

// IDEA: False = the non-halting function, true = everything else



/*

TODO: Namespaces and other sugar.

Namespaces = named scopes!

f a b ↦ r
	x …
	y …

then allows the global scope to access the constants:

f.x
f.y


Simmilarly, one can define a namespace:

f ↦
	x …
	y …


TODO: Annotation mechanism, attach metadata to a symbol


TODO: importing namespaces:

import int Principia.Integers

int.add a b


TODO: process docstrings:

ignore “uncommented piece of code”

doc symbol “Documentation”


TODO: Reflection

a ≔ parse “some piece of source code”

doc parse “Returns a closure representing the parsed source code, all unbound variables are arguments, all exported symbols are returns.”

c ≔ link a b

doc link “Go trough all arguments of the closure a and link them to variables in scope b. Unlinked symbols remain.”

rets ≔ eval f args

doc eval “Execute f with arguments args and return the results in rets.”






TODO: Multiple implementations (algorithms) of the same function

May not differ in the function specification. Must contain full proofs. May differ in runtime resource usage.

The compiler is free to change between different implementations of a function as it sees fit (optimizing). The programmer can provide hints to direct this choice.

Alternative: Functions have a unique implementation, but one can proof equivalences, which the compiler will then use.

Suppose we have two functions, safediv and unsafediv, where the later is faster, but has the additional precondition that m ≠ 0. If the compiler can deduce statically or runtime that always m ≠ 0, then it may move to the unsafediv.



TODO: Types‽ What do we mean we we say “pre n : integer”?
That there exists functions +, -, ×, |·|, =, ≠, <, ≤, >, ≥, etc… doing the ‘expected’ thing.

They can be implemented as unspecified propositions: IsInteger(n). Dependent types can be implemented as more complex variants: IsMatrix(A, ℂ, 2, 4) to state that A is a complex valued 2 × 4 matrix.



TODO: Performance characteristics:

slowdiv n m ↦ q r
	complexity.time slowdiv log(n) + log(m) 
	complexity.memory slowdiv 2 * m




TODO: Compiler hints

intdiv n m ↦ q r
	hint intdiv hint.Inline
	hint intdiv hint.Tailrecurse
	force intdiv force.Inline


TODO: Something like the Common Lisp Object System and "The Art of the Metaobject Protocol"


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
	wcerr << L"Simple C++ interpreter for the Principia language" << endl;
	wcerr << endl;
	if(args.size() < 3) {
		wcerr << "Usage: proglang source_file function [arguments]*" << endl;
		throw std::runtime_error("Not enough arguments.");
	}
	
	// Parse file
	wcerr << L"Parsing file…" << flush;
	Parser parser;
	parser.parse(args[1]);
	DataFlowGraph* dfg = parser.dataFlowGraph();
	wcerr << endl;
	
	/*
	wcerr << endl << endl;
	foreach(const Node* node, dfg->nodes())
		wcerr << node << " " << node->out() << " " << node->in() << endl; 
	wcerr << endl << endl;
	*/
	
	/*
	// Print structure
	wcerr << L"Writing dot file…" << flush;
	DotFileWriter dfw(L"test.dot");
	dfw.contractionMode(DotFileWriter::Calls);
	dfw.write(*dfg);
	// make && ./debug Ackermann.txt PRA 2 2 && dot -Tps ./test.dot -o ./test.ps
	wcerr << endl;
	*/
	
	// To validate
	// - Take the DFG
	// - Remove first out of closures
	// - Contract Edges and Calls
	// - Verify that the resulting graph is a tree
	// - For each leaf:
	//   - Take the strongly connected component containing the closure
	//   - Verify that all cycles go through the closure
	//   - Contract the strongly connected component to  
	
	/*
	// Validate
	wcerr << L"Validating structure…" << flush;
	Validator validator(dfg);
	validator.validate();
	validator.print();
	wcerr << endl;
	*/
	
	// Close over closures
	// Create constant closures
	// Repeat untill fixed point
	LambdaLifter ll(dfg);
	ConstantClosure cc(dfg);
	do {
		ll.anotateClosures();
		cc.anotateClosures();
	} while (!(ll.fixedPoint() && cc.fixedPoint()));
	
	
	// Topological sort the bodies of functions
	TopologicalSorter ts(dfg);
	ts.sortClosures();
	wcerr << endl;
	
	//
	//  Parse the command line
	//
	
	// Get the edge
	Edge* edge = 0;
	foreach(Node* node, dfg->nodes()) {
		for(int i = 0; i < node->outArrity(); ++i) {
			Edge* tedge = node->out(i);
			if(tedge == 0)
				continue;
			if(!tedge->has<IdentifierProperty>())
				continue;
			if(tedge->get<IdentifierProperty>().value() == args[2])
				edge = tedge;
		}
	}
	if(edge == 0) {
		wcerr << L"Error could not edge function " << args[2] << endl;
		return -1;
	}
	wcerr << "Evaluating edge " << edge << endl;
	
	// Evaluate edge
	Interpreter interpreter;
	Value value = interpreter.evaluateEdge(edge);
	wcerr << "Result is " << value << endl;
	
	if(value.kind == Value::Function) {
	
		// Parse arguments
		vector<Value> arguments;
		for(int i = 3; i < args.size(); ++i) {
			std::wstringstream ss(args[i]);
			sint64 value;
			ss >> value;
			arguments.push_back(Value(value));
		}
		wcerr << "Calling with arguments: " << arguments << endl;
		
		// Call function
		vector<Value> results = Interpreter::evaluateFunction(value.closure()->node(), value.closure()->context(), arguments);
		wcout << "Resulted in: " << results << endl;
	}
	
	return 0;
}

