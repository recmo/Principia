#include "DataFlowGraphCompiler.h"
#include <Parser/Parser.h>
#include <Parser/ImplicitOutputAdder.h>
#include <Parser/StatementUninliner.h>
#include <Parser/IdentifierBinder.h>
#include <UnitTest++/UnitTest++.h>
#include <Utilities/testExamples.h>

SUITE(DataFlowGraphCompiler) {

TEST(Empty)
{
	ParseTree tree;
	CHECK(tree.validate());
	
	DataFlowGraphCompiler dfgc(&tree);
	dfgc.compile();
	CHECK(tree.validate());
	
	DataFlowGraph* dfg = dfgc.dataFlowGraph();
	CHECK(dfg != nullptr);
	CHECK_EQUAL(0, dfg->nodes().size());
}

TEST(ConstFunc)
{
	Parser p;
	p.parse(L"five ↦ 5");
	ParseTree* tree = p.tree();
	CHECK(tree != nullptr);
	CHECK(tree->validate());
	
	DataFlowGraphCompiler dfgc(tree);
	dfgc.compile();
	CHECK(tree->validate());
	
	DataFlowGraph* dfg = dfgc.dataFlowGraph();
	CHECK(dfg != nullptr);
	CHECK_EQUAL(1, dfg->nodes().size());
}

TEST(CallConstFunc)
{
	Parser p;
	p.parse(L"five ↦ 5\nnumber ≔ five");
	ParseTree* tree = p.tree();
	CHECK(tree != nullptr);
	CHECK(tree->validate());
		
	// Add implicit outputs
	ImplicitOutputAdder implicitOutputAdder(tree);
	implicitOutputAdder.addImplicitOutputs();
	CHECK(tree->validate());
	
	// Uninline statements
	StatementUninliner statementUninliner(tree);
	statementUninliner.uninline();
	CHECK(tree->validate());

	// Bind identifiers
	IdentifierBinder ib(tree);
	ib.bind();
	CHECK(tree->validate());

	DataFlowGraphCompiler dfgc(tree);
	dfgc.compile();
	CHECK(tree->validate());
	
	DataFlowGraph* dfg = dfgc.dataFlowGraph();
	CHECK(dfg != nullptr);
	CHECK_EQUAL(2, dfg->nodes().size());
}

TEST(CallBuiltin)
{
	Parser p;
	p.parse(L"five ≔ add 2 3");
	ParseTree* tree = p.tree();
	CHECK(tree != nullptr);
	CHECK(tree->validate());
		
	// Add implicit outputs
	ImplicitOutputAdder implicitOutputAdder(tree);
	implicitOutputAdder.addImplicitOutputs();
	CHECK(tree->validate());
	
	// Uninline statements
	StatementUninliner statementUninliner(tree);
	statementUninliner.uninline();
	CHECK(tree->validate());

	// Bind identifiers
	IdentifierBinder ib(tree);
	ib.bind();
	CHECK(tree->validate());

	DataFlowGraphCompiler dfgc(tree);
	dfgc.compile();
	CHECK(tree->validate());
	
	DataFlowGraph* dfg = dfgc.dataFlowGraph();
	CHECK(dfg != nullptr);
	CHECK_EQUAL(1, dfg->nodes().size());
}

void testExample(const string& filename)
{
	// Parse the file
	Parser p;
	p.parseFile(filename);
	ParseTree* tree = p.tree();
	CHECK(tree != nullptr);
	CHECK(tree->validate());
	tree->uniqueifyNames();
	CHECK(tree->validate());
	
	// Add implicit outputs
	ImplicitOutputAdder implicitOutputAdder(tree);
	implicitOutputAdder.addImplicitOutputs();
	CHECK(tree->validate());
	
	// Uninline statements
	StatementUninliner statementUninliner(tree);
	statementUninliner.uninline();
	CHECK(tree->validate());

	// Bind identifiers
	IdentifierBinder ib(tree);
	ib.bind();
	CHECK(tree->validate());
	
	DataFlowGraphCompiler dfgc(tree);
	dfgc.compile();
	CHECK(tree->validate());
	
	DataFlowGraph* dfg = dfgc.dataFlowGraph();
	CHECK(dfg != nullptr);
	dfg->nodes();
}

TestExamples;

} // SUITE
