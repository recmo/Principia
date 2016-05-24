#include <DFG/DataFlowGraph.h>
#include <DFG/Interpreter.h>
#include <Parser/Parser.h>
#include <Parser/IdentifierProperty.h>
#include <Parser/ImplicitOutputAdder.h>
#include <Parser/StatementUninliner.h>
#include <Parser/IdentifierScoper.h>
#include <Parser/IdentifierBinder.h>
#include <Parser/DataFlowGraphBuilder.h>
#include <Unicode/string.h>
#include <iostream>
#include <vector>
#include <memory>

sint32 Main(const std::vector<string>& args)
{
	using namespace std;
	wcerr << PROGRAM << " version " << VERSION << endl;
	
	if(args.size() < 1) {
		wcerr << "Usage: proglang source_file [function [arguments]*]" << endl;
		return -1;
	}
	
	// Parse file
	wcerr << L"Parsing file…" << flush;
	Parser parser;
	parser.parseFile(args[1]);
	ParseTree* tree = parser.tree();
	assert(tree->validate());
	wcerr << endl;
	
	// Add implicit outputs
	wcerr << L"Adding implicit outputs…" << flush;
	ImplicitOutputAdder implicitOutputAdder(tree);
	implicitOutputAdder.addImplicitOutputs();
	assert(tree->validate());
	wcerr << endl;
	
	// Uninline inline statements
	wcerr << L"Uninlining statements…" << flush;
	StatementUninliner statementUninliner(tree);
	statementUninliner.uninline();
	assert(tree->validate());
	wcerr << endl;
	
	// Uninline inline statements
	wcerr << L"Scope the identifiers…" << flush;
	IdentifierScoper identifierScoper(tree);
	identifierScoper.scope();
	assert(tree->validate());
	wcerr << endl;
	
	// Bind the identifiers
	wcerr << L"Binding identifiers…" << flush;
	IdentifierBinder ib(tree);
	ib.bind();
	wcerr << endl;
	
	// Compile to a data flow graph
	wcerr << L"Compiling data flow graph…" << flush;
	DataFlowGraphBuilder dfgc(tree);
	dfgc.compile();
	DataFlowGraph* dfg = dfgc.dataFlowGraph();
	wcerr << endl;
	
	// Get the function to call
	wcerr << L"Finding function to call…" << flush;
	std::shared_ptr<OutPort> func;
	for(auto node: dfg->nodes()) {
		if(node->type() != Node::Closure)
			continue;
		auto& out = node->out(0);
		if(!out.has<IdentifierProperty>())
			continue;
		auto label = out.get<IdentifierProperty>().value();
		if(label == args[2]) {
			func = out.shared_from_this();
			break;
		}
	}
	if(!func) {
		wcerr << L"Error could not edge function " << args[2] << endl;
		return -1;
	}
	wcerr << endl;
	
	// Parse arguments
	wcerr << "Parsing arguments…" << flush;
	Interpreter::Values arguments;
	for(int i = 3; i < args.size(); ++i)
		arguments.push_back(Value{parse<sint64>(args[i])});
	wcerr << endl;
	
	// Execute edge
	wcerr << "Calling " << func;
	wcerr << " with arguments " << arguments << L"…" << flush;
	Interpreter::Values result = Interpreter::evaluate(func->parent(), arguments);
	wcerr << endl;
	wcout << "Resulted is: " << result << endl;
	return 0;
}
