#include <DFG/DataFlowGraph.h>
#include <DFG/DataFlowGraph.test.h>
#include <DFG/Interpreter.h>
#include <DFG/Builtin.h>
#include <Utilities/pretty_print.h>
#include <UnitTest++/UnitTest++.h>

SUITE(Interpreter) {

typedef Interpreter::Values Values;
typedef Interpreter::Context Context;

sint64 eval(Node& node)
{
	const Values result = Interpreter::evaluate(node, Values{});
	CHECK_EQUAL(1, result.size());
	CHECK_EQUAL(Value::Integer, result[0].type());
	return result[0].integer();
}

sint64 eval(Node& node, sint64 a)
{
	const Values args = Values{Value{a}};
	const Values result = Interpreter::evaluate(node, args);
	CHECK_EQUAL(1, result.size());
	CHECK_EQUAL(Value::Integer, result[0].type());
	return result[0].integer();
}

sint64 eval(Node& node, sint64 a, sint64 b)
{
	const Values args = Values{Value{a}, Value{b}};
	const Values result = Interpreter::evaluate(node, args);
	CHECK_EQUAL(1, result.size());
	CHECK_EQUAL(Value::Integer, result[0].type());
	return result[0].integer();
}

std::pair<sint64, sint64> eval2(Node& node, sint64 a, sint64 b)
{
	const Values args = Values{Value{a}, Value{b}};
	const Values result = Interpreter::evaluate(node, args);
	CHECK_EQUAL(2, result.size());
	CHECK_EQUAL(Value::Integer, result[0].type());
	CHECK_EQUAL(Value::Integer, result[1].type());
	return std::make_pair(result[0].integer(), result[1].integer());
}

/*

TEST(ConstFunc)
{
	// f ↦ 1337
	auto node1 = std::make_shared<Node>(Node::Closure, 1, 1);
	node1->in(0) << Value{1337L};
	
	CHECK_EQUAL(1337, eval(node1));
}

TEST(Id1Func)
{
	// f a ↦ b
	auto node1 = std::make_shared<Node>(Node::Closure, 2, 1);
	node1->in(0) << node1->out(1);
	
	for(sint64 a = -10; a < 10; ++a)
		CHECK_EQUAL(a, eval(node1, a));
}

TEST(Id2Func)
{
	// f a b ↦ a b
	auto node1 = std::make_shared<Node>(Node::Closure, 3, 2);
	node1->in(0) << node1->out(1);
	node1->in(1) << node1->out(2);
	
	// Test
	for(sint64 a = -10; a < 10; ++a)
		for(sint64 b = -10; b < 10; ++b)
			CHECK_EQUAL(std::make_pair(a, b), eval2(node1, a, b));
}

TEST(AddFunc)
{
	// f a b ↦ s
	// s ≔ add a b
	auto node1 = std::make_shared<Node>(Node::Closure, 3, 1);
	auto node2 = std::make_shared<Node>(Node::Call, 1, 3);
	node1->in(0) << node2->out(0);
	node2->in(0) << Builtin::add;
	node2->in(1) << node1->out(1);
	node2->in(2) << node1->out(2);
	
	// Test
	for(sint64 a = -10; a < 10; ++a)
		for(sint64 b = -10; b < 10; ++b)
			CHECK_EQUAL(a + b, eval(node1, a, b));
}

*/

TEST(Conditional)
{
	Examples::Conditional dfg;
	for(sint64 a = -10; a < 10; ++a)
		CHECK_EQUAL(a == 0 ? 10 : 20, eval(dfg.node1, a));
}

TEST(Factorial)
{
	Examples::Factorial dfg;
	CHECK_EQUAL(1, eval(dfg.node1, 0));
	CHECK_EQUAL(1, eval(dfg.node1, 1));
	CHECK_EQUAL(2, eval(dfg.node1, 2));
	CHECK_EQUAL(6, eval(dfg.node1, 3));
	CHECK_EQUAL(24, eval(dfg.node1, 4));
	CHECK_EQUAL(120, eval(dfg.node1, 5));
	CHECK_EQUAL(720, eval(dfg.node1, 6));
}

} // SUITE
