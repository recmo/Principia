#include <DFG/DataFlowGraph.h>

namespace Examples {

// Conditional
// 1 f n ↦ r
// 2 	if_zero ↦ 10
// 3 	otherwise ↦ 20
// 4 	fr ≔ if n otherwise if_zero
// 5 	r ≔ fr
struct Conditional: public DataFlowGraph {
	Conditional();
	Node& node1;
	Node& node2;
	Node& node3;
	Node& node4;
	Node& node5;
};

// Factorial
// 1 fact n ↦ r
// 2 	if_zero rn ↦ 1
// 3 	otherwise rn ↦ or
// 4 		pn ≔ sub rn 1
// 5 		pr ≔ fact pn
// 6 		or ≔ mul rn pr
// 7 	fr ≔ if n otherwise if_zero
// 8 	r ≔ fr n
struct Factorial: public DataFlowGraph {
	Factorial();
	Node& node1;
	Node& node2;
	Node& node3;
	Node& node4;
	Node& node5;
	Node& node6;
	Node& node7;
	Node& node8;
};

}
