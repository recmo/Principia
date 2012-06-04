#include <Passes/StackAllocator.h>
#include "OrderProperty.h"
#include "ClosureProperty.h"
#include "StackProperty.h"
#include "ReturnStackProperty.h"
#include <Parser/ConstantProperty.h>

#define debug true

void StackAllocator::annotate()
{
	foreach(Node* node, _dfg->nodes()) {
		if(node->type() != NodeType::Closure)
			continue;
		if(!node->has<OrderProperty>())
			continue;
		
		// Collect all values on a stack
		vector<const Edge*> stack;
		stack << node->get<ClosureProperty>().edges();
		for(int i = 1; i < node->outArrity(); ++i)
			stack.push_back(node->out(i));
		// Process the function body in evaluation order
		vector<const Node*> body = node->get<OrderProperty>().nodes();
		foreach(const Node* eval, body){
			
			// Find the positions of the sources
			vector<const Edge*> sources = (eval->type() == NodeType::Call) ? eval->in() : eval->get<ClosureProperty>().edges();
			vector<int> stackPositions(sources.size(), -1);
			for(int i = 0; i < sources.size(); ++i) {
				if(sources[i]->has<ConstantProperty>())
					continue;
				for(int j = 0; j < stack.size(); ++j) {
					if(sources[i] == stack[j]) {
						stackPositions[i] = j;
						break;
					}
				}
				assert(stackPositions[i] >= 0);
			}
			const_cast<Node*>(eval)->set(StackProperty(stackPositions));
			if(debug) {
				wcerr << eval << " parameters " << sources << endl;
				wcerr << eval << " positions  " << stackPositions << endl;
			}
			
			// Add the returns to the stack
			if(eval->type() == NodeType::Call)
				stack << eval->out();
			else
				stack << eval->out(0);
		}
		if(debug)
			wcerr << node << " stack "  << stack << endl;
		
		// Collect the return values from the stack
		vector<int> returnIndices;
		returnIndices.reserve(node->inArrity());
		for(int i = 0; i < node->inArrity(); ++i) {
			const Edge* returnValue = node->in(i);
			if(returnValue->has<ConstantProperty>()) {
				returnIndices << -1;
				continue;
			}
			for(int j = 0; i < stack.size(); ++j) {
				if(returnValue == stack[j]) {
					returnIndices << j;
					break;
				}
			}
		}
		node->set(ReturnStackProperty(returnIndices));
		if(debug)
			wcerr << node << " returns "  << returnIndices << endl;
	}
}
