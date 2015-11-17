#include "fixups.h"
#include "Parser/Parser.h"
#include "Passes/Value.h"
#include "Passes/Validator.h"
#include "Parser/Parser.h"
#include "DFG/DataFlowGraph.h"
#include "DFG/CongruenceComparator.h"
#include "Parser/IdentifierProperty.h"
#include "Parser/IdentifierBinder.h"
#include "Parser/DataFlowGraphCompiler.h"
#include "Parser/MetaMath/MetaMathParser.h"
#include "Parser/StatementUninliner.h"
#include "Parser/ImplicitOutputAdder.h"
#include "Parser/IdentifierScoper.h"
#include "Passes/DotFileWriter.h"
#include "Passes/ClosureCloser.h"
#include "Passes/ConstantClosure.h"
#include "Passes/ConstantCall.h"
#include "Passes/StackCompiler.h"
#include "Passes/LlvmCompiler.h"
#include "Passes/NativeProperty.h"
#include "Passes/ClosureProperty.h"
#include "Passes/EscapeAnalysis.h"
#include "Verifier/Verifier.h"
#include <fstream>
#include <cmath>

sint32 Main(const vector<string>& args)
{
	wcerr << L"Welcome to the Principia language" << endl;
	//Interactive session;
	//session.loop();
	//return 0;
	
	wcerr << endl;
	if(args.size() < 1) {
		wcerr << "Usage: proglang source_file [function [arguments]*]" << endl;
		throw std::runtime_error("Not enough arguments.");
	}
	
	// Parse file
	wcerr << L"Parsing file…" << flush;
	Parser parser;
	parser.parse(args[1]);
	ParseTree* tree = parser.tree();
	assert(tree->validate());
	wcerr << endl;
	
	const bool parseDebug = false;
	if(parseDebug) {
		wcout << "----------------------------" << endl;
		tree->print(wcout);
		wcout << "----------------------------" << endl;
	}
	
	// Add implicit outputs
	wcerr << L"Adding implicit outputs…" << flush;
	ImplicitOutputAdder implicitOutputAdder(tree);
	implicitOutputAdder.addImplicitOutputs();
	assert(tree->validate());
	wcerr << endl;
	
	if(parseDebug) {
		wcout << "----------------------------" << endl;
		tree->print(wcout);
		wcout << "----------------------------" << endl;
	}
	
	// Uninline inline statements
	wcerr << L"Uninlining statements…" << flush;
	StatementUninliner statementUninliner(tree);
	statementUninliner.uninline();
	assert(tree->validate());
	wcerr << endl;
	
	if(parseDebug) {
		wcout << "----------------------------" << endl;
		tree->print(wcout);
		wcout << "----------------------------" << endl;
	}
	
	// Uninline inline statements
	wcerr << L"Scope the identifiers…" << flush;
	IdentifierScoper identifierScoper(tree);
	identifierScoper.scope();
	assert(tree->validate());
	wcerr << endl;
	
	if(parseDebug) {
		wcout << "----------------------------" << endl;
		tree->print(wcout);
		wcout << "----------------------------" << endl;
	}
	
	// Bind the identifiers
	wcerr << L"Binding identifiers…" << flush;
	IdentifierBinder ib(tree);
	ib.bind();
	wcerr << endl;
	
	if(parseDebug) {
		wcout << "----------------------------" << endl;
		tree->print(wcout);
		wcout << "----------------------------" << endl;
	}
	
	// Compile to a data flow graph
	wcerr << L"Compiling data flow graph…" << flush;
	DataFlowGraphCompiler dfgc(tree);
	dfgc.compile();
	DataFlowGraph* dfg = dfgc.dataFlowGraph();
	wcerr << endl;
	
	// Print structure
	if(false) {
		wcerr << L"Writing dot file…" << flush;
		DotFileWriter dfw(L"test.dot");
		dfw.write(*dfg);
		// make && ./debug Ackermann.txt PRA 2 2 && dot -Tps ./test.dot -o ./test.ps
		wcerr << endl;
	}
	
	// To validate
	// - Take the DFG
	// - Remove first out of closures
	// - Contract Edges and Calls
	// - Verify that the resulting graph is a tree (closure tree?)
	// - For each leaf:
	//   - Take the strongly connected component containing the closure
	//   - Verify that all cycles go through the closure
	//   - Contract the strongly connected component to  
	
	// Validate
	if(false) { /// @TODO
		wcerr << L"Validating structure…" << flush;
		Validator validator(dfg);
		validator.validate();
		validator.print();
		wcerr << endl;
	}
	
	// Close over closures
	// Create constant closures
	// Repeat until fixed point
	ClosureCloser ll(*dfg);
	ConstantClosure cclosure(dfg);
	ConstantCall ccall(dfg);
	do {
		wcerr << L"Determining closures…" << flush;
		ll.anotateClosures();
		wcerr << endl;
		
		wcerr << L"Finding constant closures…" << flush;
		cclosure.anotateClosures();
		wcerr << endl;
		
		wcerr << L"Constant call creation…" << flush;
		/// @TODO: BUG
		//ccall.anotateCalls();
		wcerr << endl;
		
	} while (!(ll.fixedPoint() && cclosure.fixedPoint() && ccall.fixedPoint()));
	
	// Compile to a stack machine
	wcerr << L"Compiling to stack machine language…" << flush;
	StackCompiler ts(dfg);
	ts.sortClosures();
	wcerr << endl;
	
	// Escape analyisis
	wcerr << L"Escape analysis…" << flush;
	EscapeAnalysis ea(dfg);
	ea.analyse();
	wcerr << endl;
	
	// Verification
	wcerr << L"Verifiying…" << flush;
	Verifier v(dfg);
	v.verify();
	wcerr << endl;
	
	/*
	wcerr << endl << endl;
	for(const Node* node: dfg->nodes()) {
		wcerr << node << " " << node->constOut() << " " << node->constIn() << endl;
		node->printProperties(wcerr);
		for(const Edge* edge: node->constOut()) {
			wcerr << "    " << edge << endl;
			edge->printProperties(wcerr);
		}
		wcerr << endl;
	}
	wcerr << endl << endl;
	*/
	
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
	std::shared_ptr<Edge> edge;
	for(auto node: dfg->nodes()) {
		if(node->type() != NodeType::Closure)
			continue;
		auto out = node->out(0);
		if(!out->has<IdentifierProperty>())
			continue;
		auto label = out->get<IdentifierProperty>().value();
		if(label == args[2]) {
			edge = edge;
			break;
		}
	}
	if(!edge) {
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
	std::shared_ptr<Node> closure = edge->source();
	assert(closure->has<NativeProperty>());
	assert(closure->has<ClosureProperty>() && closure->get<ClosureProperty>().edges().empty());
	vector<Value> results = closure->get<NativeProperty>().call(arguments);
	wcerr << endl;
	wcout << "Resulted is: " << results << endl;
	
	return 0;
}
