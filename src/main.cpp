#include "fixups.h"
#include "Parser/Parser.h"
#include "Interpreter/Value.h"
#include "Passes/Validator.h"
#include "Parser/Parser.h"
#include "DFG/DataFlowGraph.h"
#include "Parser/IdentifierProperty.h"
#include "Parser/IdentifierBinder.h"
#include "Parser/DataFlowGraphCompiler.h"
#include "Passes/DotFileWriter.h"
#include "Passes/LambdaLifter.h"
#include "Passes/ConstantClosure.h"
#include "Passes/TopologicalSorter.h"
#include "Passes/StackAllocator.h"
#include "Passes/LlvmCompiler.h"
#include "Passes/NativeProperty.h"
#include <fstream>
#include <cmath>

/// TODO: Add mutual recursion to llvm compiler
/// TODO: Find a contrived example where the validity of the mutual recursion depends on an unsolved problem.
///       f x ↦ (≔(≔ if (↦(≔ complicated_function_1 x)) (↦(≔ g x)) (↦x))
///       g x ↦ (≔(≔ if (↦(≔ complicated_function_2 x)) (↦(≔ f x)) (↦x))
/// Then add a variable to force the closure to be dynamic.
/// TODO: Find a contrived example where a partially filled closure might still be executed.
/// TODO: Proof that finding an executable closure storing order is equivalent to the halting problem.

/// TODO: McCarthy 91 function

// M n ↦ (≔(≔ if (≔ greater n 100) (↦(≔ sub n 10)) (↦(≔ M (≔ M (≔ add n 11))))))


/// TODO: Add proof language

/*

Hoare Logic:

f a ↦ b
	Theorems A(a) about a
	Derivations / proofs
	Theorems B(b) about b

Theorems Y(y) about y
x ≔ f y
	Verify A(y) ⊆ Y(y)
	Therefore B(x)

How do we do derivations?

IDEA: We can use abstract functions, i.e. functions without implementations
 to represent (non-constructive) theorems.
This function should generally return just the input variables,
however, it may return new values. This would be an existence proof!

How do we do axioms?

IDEA: We omit the derivations from the function.

This gives four kinds of functions

- Without implementation and without derivation. (no-op)
- With implementation and without derivation. (unproven construction)
- Without implementation and with derivation. (abstract/pure/nonconstructive proof)
- With implementation and with derivation. (plain function with proof)

*/


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
[parse rule:  if #1 then #2 else #3  ↦ (≔(≔if #1 (↦ #2) (↦#3))) ]
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
	wcerr << endl;
	
	// Fetch the parse tree
	ParseTree* tree = parser.tree();
	
	// Bind the identifiers
	wcerr << L"Binding identifiers…" << flush;
	IdentifierBinder ib(tree);
	ib.bind();
	wcerr << endl;
	
	// Compile to a data flow graph
	wcerr << L"Compiling data flow graph…" << flush;
	DataFlowGraphCompiler dfgc(tree);
	dfgc.compile();
	DataFlowGraph* dfg = dfgc.dataFlowGraph();
	wcerr << endl;

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
	
	// Validate
	/*
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
		wcerr << L"Lambda lifting…" << flush;
		ll.anotateClosures();
		wcerr << endl;
		
		wcerr << L"Constant closure creation…" << flush;
		cc.anotateClosures();
		wcerr << endl;
	} while (!(ll.fixedPoint() && cc.fixedPoint()));
	
	wcerr << endl << endl;
	foreach(const Node* node, dfg->nodes()) {
		wcerr << node << " " << node->out() << " " << node->in() << endl;
		node->printProperties(wcerr);
		foreach(const Edge* edge, node->out()) {
			wcerr << "    " << edge << endl;
			edge->printProperties(wcerr);
		}
		wcerr << endl;
	}
	wcerr << endl << endl;
	
	// Topological sort the bodies of functions
	wcerr << L"Sorting closure bodies topologically…" << flush;
	TopologicalSorter ts(dfg);
	ts.sortClosures();
	wcerr << endl;
	
	wcerr << endl << endl;
	foreach(const Node* node, dfg->nodes()) {
		wcerr << node << " " << node->out() << " " << node->in() << endl;
		node->printProperties(wcerr);
		foreach(const Edge* edge, node->out()) {
			wcerr << "    " << edge << endl;
			edge->printProperties(wcerr);
		}
		wcerr << endl;
	}
	wcerr << endl << endl;
	
	// Compile to LLVM !
	wcerr << L"Compiling to native code using LLVM…" << flush;
	LlvmCompiler lc(dfg);
	lc.compile();
	wcerr << endl;
	
	//
	//  Parse the command line
	//
	
	// Get the edge
	wcerr << L"Finding function to call…" << flush;
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
	wcerr << endl;
	
	// Parse arguments
	wcerr << "Parsing arguments…" << flush;
	vector<Value> arguments;
	for(int i = 3; i < args.size(); ++i) {
		std::wstringstream ss(args[i]);
		sint64 value;
		ss >> value;
		arguments.push_back(Value(value));
	}
	wcerr << endl;
	
	// Execute edge
	wcerr << "Calling " << edge << " with arguments " << arguments << L"…" << flush;
	const Node* closure = edge->source();
	assert(closure->has<NativeProperty>());
	vector<Value> results = closure->get<NativeProperty>().call(arguments);
	wcerr << endl;
	wcout << "Resulted is: " << results << endl;
	
	return 0;
}

