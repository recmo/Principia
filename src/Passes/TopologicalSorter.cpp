#include <Passes/TopologicalSorter.h>
#include "ClosureProperty.h"
#include "OrderProperty.h"
#include <DFG/DataFlowGraph.h>
#include <DFG/Node.h>
#include <Parser/ConstantProperty.h>

void TopologicalSorter::sortClosures()
{
	foreach(Node* node, _dfg->nodes()) {
		if(node->type() != NodeType::Closure)
			continue;
		sortClosure(node);
	}
}

void TopologicalSorter::sortClosure(Node* closureNode)
{
	assert(closureNode->has<ClosureProperty>());
	const vector<const Edge*>& closure = closureNode->get<ClosureProperty>().edges();
	vector<const Edge*> returns = closureNode->in();
	vector<const Edge*> args;
	for(int i = 1; i < closureNode->outArrity(); ++i)
		args.push_back(closureNode->out(i));
	
	vector<const Node*> ordered;
	
	// Start from the returns
	foreach(const Edge* edge, returns) {
		if(edge->has<ConstantProperty>())
			continue;
		if(contains(closure, edge))
			continue;
		if(contains(args, edge))
			continue;
		const Node* source = edge->source();
		if(contains(ordered, source))
			continue;
		ordered.push_back(source);
	}
	
	// Now go backwards untill everything is added
	bool fixedpoint;
	do {
		fixedpoint = true;
		foreach(const Node* node, ordered) {
			
			// Find the sources for this node
			vector<const Edge*> sources;
			if(node->type() == NodeType::Call)
				sources = node->in();
			else
				sources = node->get<ClosureProperty>().edges();
			
			// And go back from there
			foreach(const Edge* edge, sources) {
				if(edge->has<ConstantProperty>())
					continue;
				if(contains(closure, edge))
					continue;
				if(contains(args, edge))
					continue;
				const Node* source = edge->source();
				if(contains(ordered, source))
					continue;
				ordered.push_back(source);
				fixedpoint = false;
			}
		}
	} while(!fixedpoint);
	
	// The reversed list is the evaluation order
	std::reverse(ordered.begin(), ordered.end());
	closureNode->set(OrderProperty(ordered));
}

