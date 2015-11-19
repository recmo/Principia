#include <Passes/ClosureCloser.h>
#include <Passes/ClosureProperty.h>
#include <DFG/DataFlowGraph.h>
#include <DFG/DataFlowGraph.test.h>
#include <DFG/Value.h>
#include <DFG/Builtin.h>
#include <UnitTest++/UnitTest++.h>

SUITE(ClosureCloser) {

TEST(Factorial)
{
	Examples::Factorial dfg;
	
	ClosureCloser cc(dfg);
	cc.anotateClosures();
	
	// All and only closure nodes are annotated
	for(auto node: dfg.nodes()) {
		if(node->type() == Node::Closure)
			CHECK(node->has<ClosureProperty>());
		else
			CHECK(!node->has<ClosureProperty>());
	}
	
	CHECK_EQUAL(1, dfg.node1.get<ClosureProperty>().closureSet().size());
	CHECK_EQUAL(
		dfg.node2.out(0).shared_from_this(),
		dfg.node1.get<ClosureProperty>().closureSet()[0]);
	CHECK_EQUAL(0, dfg.node2.get<ClosureProperty>().closureSet().size());
	CHECK_EQUAL(1, dfg.node3.get<ClosureProperty>().closureSet().size());
	CHECK_EQUAL(
		dfg.node2.out(0).shared_from_this(),
		dfg.node3.get<ClosureProperty>().closureSet()[0]);
}

} // SUITE
