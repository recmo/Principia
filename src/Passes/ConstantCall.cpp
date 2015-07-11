#include "ConstantCall.h"
#include <DFG/DataFlowGraph.h>
#include <DFG/Node.h>
#include <Parser/ConstantProperty.h>

#define debug true

void ConstantCall::anotateCalls()
{
	_fixedPoint = true;
	for(Node* node: _dfg->nodes()) {
		if(node->type() != NodeType::Call)
			continue;
		if(node->has<ConstantProperty>())
			continue;
		
		// If all inputs are constant, the call is constant
		bool isConstant = true;
		for(uint i = 0; i < node->inArity(); ++i) {
			if(!node->in(i)->has<ConstantProperty>()) {
				isConstant = false;
				break;
			}
		}
		if(!isConstant)
			continue;
		
		if(debug)
			wcerr << node << " is run-time constant" << endl;
		_fixedPoint = false;
		
		// If the call is constant, mark it, and all its outputs so
		/// @todo Values
		node->set(ConstantProperty(Value()));
		for(uint i = 0; i < node->outArity(); ++i)
			node->out(i)->set(ConstantProperty(Value()));
	}
}
