#include <DFG/DataFlowGraph.h>
#include <DFG/DataFlowGraph.test.h>
#include <DFG/Value.h>
#include <DFG/Builtin.h>
#include <UnitTest++/UnitTest++.h>

namespace Examples {

// 1 f n ↦ r
// 2 	if_zero ↦ 10
// 3 	otherwise ↦ 20
// 4 	fr ≔ if n otherwise if_zero
// 5 	r ≔ fr
Conditional::Conditional()
: DataFlowGraph()
, node1{make_closure(2, 1)}
, node2{make_closure(1, 1)}
, node3{make_closure(1, 1)}
, node4{make_call(1, 4)}
, node5{make_call(1, 1)}
{
	node1.in(0) << node5.out(0);
	node2.in(0) << Value{10L};
	node3.in(0) << Value{20L};
	node4.in(0) << Builtin::if_;
	node4.in(1) << node1.out(1);
	node4.in(2) << node3.out(0);
	node4.in(3) << node2.out(0);
	node5.in(0) << node4.out(0);
}

// 1 fact n ↦ r
// 2 	if_zero rn ↦ 1
// 3 	otherwise rn ↦ or
// 4 		pn ≔ sub rn 1
// 5 		pr ≔ fact pn
// 6 		or ≔ mul rn pr
// 7 	fr ≔ if n otherwise if_zero
// 8 	r ≔ fr n
Factorial::Factorial()
: DataFlowGraph()
, node1{make_closure(2, 1)}
, node2{make_closure(2, 1)}
, node3{make_closure(2, 1)}
, node4{make_call(1, 3)}
, node5{make_call(1, 2)}
, node6{make_call(1, 3)}
, node7{make_call(1, 4)}
, node8{make_call(1, 2)}
{
	node1.in(0) << node8.out(0);
	node2.in(0) << Value(1L);
	node3.in(0) << node6.out(0);
	node4.in(0) << Builtin::sub;
	node4.in(1) << node3.out(1);
	node4.in(2) << Value(1L);
	node5.in(0) << node1.out(0);
	node5.in(1) << node4.out(0);
	node6.in(0) << Builtin::mul;
	node6.in(1) << node3.out(1);
	node6.in(2) << node5.out(0);
	node7.in(0) << Builtin::if_;
	node7.in(1) << node1.out(1);
	node7.in(2) << node3.out(0);
	node7.in(3) << node2.out(0);
	node8.in(0) << node7.out(0);
	node8.in(1) << node1.out(1);
}

}

SUITE(DataFlowGraph) {

TEST(Conditional)
{
	Examples::Conditional dfg;
	CHECK_EQUAL(5, dfg.nodes().size());
}

TEST(Factorial)
{
	Examples::Factorial dfg;
	CHECK_EQUAL(8, dfg.nodes().size());
}

} // SUITE
