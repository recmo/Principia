# DataFlowGraph

Programs are represented by a data structure similar to a directed graph. Every
call and closure instruction is represented by a node. These nodes a joined
together by edges.

Given a call instruction like

	a b ≔ f n m

Given an closure instruction like

	f a b ↦ n m

The graph will have a node with three outgoing edges and two incoming edges. The
body of function will connect the outgoing edges to the two incoming edges,
closing the loop.

Now consider the factorial function:

	fact n ↦ r
		if_zero d ↦ 1
		otherwise d ↦ or
			np ≔ sub n 1
			rr ≔ fact np
			or ≔ mul n rr
		fr ≔ if n otherwise if_zero
		r ≔ fr n

As we can tell by the number of `≔` and `↦` signs, the data flow graph has 8
nodes. Three closures and five calls.

There are also five constants. The constant `1` appears twice and the external
functions `sub`, `mul` and `if` are used.

	fact n ↦ r
		if_zero d ↦ 1
		otherwise d ↦ or
			np ≔ sub n 1
			rr ≔ fact np
			or ≔ mul n rr
		fr ≔ if n otherwise if_zero
		r ≔ fr n

In the data flow graph this will be represented as

	Node node1(Node::Closure, 2, 1);
	Node node2(Node::Closure, 2, 1);
	Node node3(Node::Closure, 2, 1);
	Node node4(Node::Call, 1, 3);
	Node node5(Node::Call, 1, 2);
	Node node6(Node::Call, 1, 3);
	Node node7(Node::Call, 1, 4);
	Node node8(Node::Call, 1, 2);
	node1.in(0) << node8.out(0);
	node2.in(0) << Value(1);
	node3.in(0) << node6.out(0);
	node4.in(0) << Builtin::sub;
	node4.in(1) << node1.out(1);
	node4.in(2) << Value(1);
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

