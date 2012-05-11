#include "ConstantClosure.h"
#include "ClosureProperty.h"
#include <DFG/DataFlowGraph.h>
#include <DFG/Node.h>
#include <Parser/ConstantProperty.h>
#include <Interpreter/Closure.h>

#define debug false

void ConstantClosure::anotateClosures()
{
	_fixedPoint = true;
	foreach(Node* node, _dfg->nodes()) {
		if(node->type() != NodeType::Closure)
			continue;
		if(!node->has<ClosureProperty>())
			continue;
		if(!node->get<ClosureProperty>().edges().empty())
			continue;
		if(node->outArrity() < 1)
			continue;
		if(node->out(0)->has<ConstantProperty>())
			continue;
		
		// We now have an closure node with an empty list of closures
		// Make it constant!
		node->out(0)->set(ConstantProperty(Value(new Closure(node))));
		if(debug)
			wcerr << node << " is constant " <<  node->out(0)->get<ConstantProperty>().value() << endl;
		
		_fixedPoint = false;
	}
}
