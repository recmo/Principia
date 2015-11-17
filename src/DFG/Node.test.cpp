#include "Node.h"
#include <UnitTest++/UnitTest++.h>
#include <DFG/Value.h>
#include <DFG/Builtin.h>
#include <Utilities/exceptions.h>

SUITE(Node) {

void construct(Node::NodeType type, uint out, uint in)
{
	auto shared = std::make_shared<Node>(type, out, in);
	Node& node = *shared;
	const Node& cnode = node;
	CHECK_EQUAL(type, node.type());
	CHECK_EQUAL(out, node.outArity());
	CHECK_EQUAL(in, node.inArity());
	for(uint i = 0; i < out; ++i) {
		node.out(i);
		CHECK_EQUAL(shared, node.out(i).parent().shared_from_this());
		CHECK_EQUAL(i, node.out(i).index());
		CHECK_EQUAL(0, node.out(i).sinks().size());
	}
	for(uint i = 0; i < in; ++i) {
		node.in(i);
		CHECK_EQUAL(shared, node.in(i).parent().shared_from_this());
		CHECK_EQUAL(i, node.in(i).index());
		CHECK(nullptr == node.in(i).source());
	}
	CHECK_THROW(node.out(out), index_out_of_range);
	CHECK_THROW(node.out(out + 1), index_out_of_range);
	CHECK_THROW(node.out(-1), index_out_of_range);
	CHECK_THROW(node.in(in), index_out_of_range);
	CHECK_THROW(node.in(in + 1), index_out_of_range);
	CHECK_THROW(node.in(-1), index_out_of_range);
	CHECK_EQUAL(type, cnode.type());
	CHECK_EQUAL(out, cnode.outArity());
	CHECK_EQUAL(in, cnode.inArity());
	for(uint i = 0; i < out; ++i) {
		cnode.out(i);
		CHECK_EQUAL(shared, cnode.out(i).parent().shared_from_this());
		CHECK_EQUAL(i, cnode.out(i).index());
		CHECK_EQUAL(0, cnode.out(i).sinks().size());
	}
	for(uint i = 0; i < in; ++i) {
		cnode.in(i);
		CHECK_EQUAL(shared, cnode.in(i).parent().shared_from_this());
		CHECK_EQUAL(i, cnode.in(i).index());
		CHECK(nullptr == cnode.in(i).source());
	}
	CHECK_THROW(cnode.out(out), index_out_of_range);
	CHECK_THROW(cnode.out(out + 1), index_out_of_range);
	CHECK_THROW(cnode.out(-1), index_out_of_range);
	CHECK_THROW(cnode.in(in), index_out_of_range);
	CHECK_THROW(cnode.in(in + 1), index_out_of_range);
	CHECK_THROW(cnode.in(-1), index_out_of_range);
}

TEST(Construct)
{
	construct(Node::Closure, 1, 1);
	construct(Node::Closure, 1, 2);
	construct(Node::Closure, 2, 0);
	construct(Node::Closure, 2, 1);
	construct(Node::Closure, 2, 2);
	construct(Node::Call, 0, 1);
	construct(Node::Call, 0, 2);
	construct(Node::Call, 1, 1);
	construct(Node::Call, 1, 2);
	construct(Node::Call, 2, 1);
	construct(Node::Call, 2, 2);
	CHECK_THROW(Node(Node::Closure, 0, 0), assertion_failed);
	CHECK_THROW(Node(Node::Closure, 0, 1), assertion_failed);
	CHECK_THROW(Node(Node::Closure, 0, 2), assertion_failed);
	CHECK_THROW(Node(Node::Call, 0, 0), assertion_failed);
	CHECK_THROW(Node(Node::Call, 1, 0), assertion_failed);
	CHECK_THROW(Node(Node::Call, 2, 0), assertion_failed);
}

TEST(Connect)
{
	Node node1(Node::Closure, 1, 1);
	node1.in(0) << node1.out(0);
	
	Node node2(Node::Closure, 1, 1);
	node2.in(0) << node1.out(0);
	node1.in(0) << node2.out(0);
	
	Node node3(Node::Closure, 1, 1);
	node3.in(0) << node2.out(0);
	node1.in(0) << node3.out(0);
	
	node1.in(0) << node1.out(0);
	node2.in(0) << node1.out(0);
	node3.in(0) << node1.out(0);
}

TEST(ConnectValues)
{
	Node node(Node::Call, 0, 5);
	node.in(0) << Value{};
	node.in(1) << Value{1L};
	node.in(2) << Value{1.0};
	node.in(3) << Builtin::if_;
	node.in(3) << Value{L"Test"};
}

TEST(Factorial)
{
	Node node1(Node::Closure, 2, 1);
	Node node2(Node::Closure, 2, 1);
	Node node3(Node::Closure, 2, 1);
	Node node4(Node::Call, 1, 3);
	Node node5(Node::Call, 1, 2);
	Node node6(Node::Call, 1, 3);
	Node node7(Node::Call, 1, 4);
	Node node8(Node::Call, 1, 2);
	node1.in(0) << node8.out(0);
	node2.in(0) << Value(1L);
	node3.in(0) << node6.out(0);
	node4.in(0) << Builtin::sub;
	node4.in(1) << node1.out(1);
	node4.in(2) << Value(1L);
	node5.in(0) << node1.out(0);
	node5.in(1) << node4.out(0);
	node6.in(0) << Builtin::mul;
	node6.in(1) << node1.out(1);
	node6.in(2) << node5.out(0);
	node7.in(0) << Builtin::if_;
	node7.in(1) << node1.out(0);
	node7.in(2) << node3.out(0);
	node7.in(3) << node2.out(0);
	node8.in(0) << node7.out(0);
	node8.in(0) << node1.out(1);
}

} // SUITE
