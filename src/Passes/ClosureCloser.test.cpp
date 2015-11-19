#include <Passes/ClosureCloser.h>
#include <Passes/ClosureProperty.h>
#include <DFG/DataFlowGraph.h>
#include <DFG/DataFlowGraph.test.h>
#include <DFG/Value.h>
#include <DFG/Builtin.h>
#include <UnitTest++/UnitTest++.h>

SUITE(ClosureCloser) {

void annotate(DataFlowGraph& dfg)
{
	ClosureCloser cc(dfg);
	cc.anotateClosures();
	
	// All and only closure nodes are annotated
	for(auto node: dfg.nodes()) {
		if(node->type() == Node::Closure)
			CHECK(node->has<ClosureProperty>());
		else
			CHECK(!node->has<ClosureProperty>());
	}
}

TEST(Conditional)
{
	// 1 f n ↦ r
	// 2 	if_zero ↦ 10
	// 3 	otherwise ↦ 20
	// 4 	fr ≔ if n otherwise if_zero
	// 5 	r ≔ fr
	Examples::Conditional dfg;
	annotate(dfg);
	
	CHECK_EQUAL(2, dfg.node1.get<ClosureProperty>().closureSet().size());
	CHECK_EQUAL(0, dfg.node2.get<ClosureProperty>().closureSet().size());
	CHECK_EQUAL(0, dfg.node3.get<ClosureProperty>().closureSet().size());
}

TEST(Factorial)
{
	// 1 fact n ↦ r
	// 2 	if_zero rn ↦ 1
	// 3 	otherwise rn ↦ or
	// 4 		pn ≔ sub rn 1
	// 5 		pr ≔ fact pn
	// 6 		or ≔ mul rn pr
	// 7 	fr ≔ if n otherwise if_zero
	// 8 	r ≔ fr n
	Examples::Factorial dfg;
	annotate(dfg);
	
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
