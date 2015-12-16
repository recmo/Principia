#include <Parser/DataFlowGraphCompiler.h>
#include <Parser/Parser.h>
#include <Parser/ImplicitOutputAdder.h>
#include <Parser/IdentifierScoper.h>
#include <Parser/IdentifierBinder.h>
#include <Parser/StatementUninliner.h>
#include <Parser/DataFlowGraphBuilder.h>

DataFlowGraph* DataFlowGraphCompiler::compileFile(const string& filename)
{
	Parser parser;
	parser.parseFile(filename);
	return compile(parser.tree());
}

DataFlowGraph* DataFlowGraphCompiler::compileString(const string& contents)
{
	Parser parser;
	parser.parseString(contents);
	return compile(parser.tree());
}

DataFlowGraph* DataFlowGraphCompiler::compileString(const std::string& bytes)
{
	Parser parser;
	parser.parseString(bytes);
	return compile(parser.tree());
}

DataFlowGraph* DataFlowGraphCompiler::compile(std::istream& stream)
{
	Parser parser;
	parser.parse(stream);
	return compile(parser.tree());
}

DataFlowGraph* DataFlowGraphCompiler::compile(ParseTree* tree)
{
	assert(tree->validate());
	
	// Add implicit outputs
	ImplicitOutputAdder implicitOutputAdder(tree);
	implicitOutputAdder.addImplicitOutputs();
	assert(tree->validate());
	
	// Uninline inline statements
	StatementUninliner statementUninliner(tree);
	statementUninliner.uninline();
	assert(tree->validate());
	
	// Uninline inline statements
	IdentifierScoper identifierScoper(tree);
	identifierScoper.scope();
	assert(tree->validate());
	
	// Bind the identifiers
	IdentifierBinder ib(tree);
	ib.bind();
	assert(tree->validate());
	
	// Compile to a data flow graph
	DataFlowGraphBuilder dfgc(tree);
	dfgc.compile();
	DataFlowGraph* dfg = dfgc.dataFlowGraph();
	
	// MORE passes
	
	return dfg;
}
