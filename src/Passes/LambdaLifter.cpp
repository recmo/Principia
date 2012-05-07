#include "Passes/LambdaLifter.h"
#include <DFG/DataFlowGraph.h>
#include <DFG/Node.h>
#include <DFG/Edge.h>
#include <Parser/ClosureProperty.h>
#include <Parser/SourceProperty.h>
#include <Parser/IdentifierProperty.h>
#include <Parser/ConstantProperty.h>

void LambdaLifter::anotateClosures()
{
	foreach(Node* node, _dfg->nodes()) {
		if(node->type() != NodeType::Closure)
			continue;
		anotateClosure(node);
	}
}

void LambdaLifter::anotateClosure(Node* closureNode)
{
	assert(closureNode->type() == NodeType::Closure);
	
	// Calculate internal edges
	vector<Edge*> internal;
	internal.push_back(closureNode->out(0));
	for(int i = 0; i < closureNode->outArrity(); ++i)
		recurseOut(closureNode->out(i), &internal);
	wcerr << closureNode << " internal " << internal << endl;
	
	// Calculate border edges
	vector<const Edge*> border;
	for(int i = 0; i < internal.size(); ++i) {
		Node* source = internal[i]->source();
		for(int j = 0; j < source->inArrity(); ++j) {
			if(contains(internal, source->in(j)))
				continue;
			if(source->in(j)->has<ConstantProperty>())
				continue;
			
			// Do not close over top level functions?
			
			if(source->in(j)->isFunction())
				continue;
			
			
			border.push_back(source->in(j));
		}
	}
	wcerr << closureNode << " border " << border << endl;
	
	/// TODO: Calculate the lazy set
	
	// Set the closure property
	ClosureProperty cp(border);
	closureNode->set(cp);
}

void LambdaLifter::recurseOut(Edge* edge, vector<Edge*>* edges)
{
	if(contains(*edges, edge))
		return;
	edges->push_back(edge);
	foreach(Node* node, edge->sinks())
		recurseOut(node, edges);
}

void LambdaLifter::recurseOut(Node* node, vector<Edge*>* edges)
{
	for(int i = 0; i < node->outArrity(); ++i)
		recurseOut(node->out(i), edges);
}

void LambdaLifter::recurseIn(Edge* edge, std::vector<Edge*>* edges)
{
	if(contains(*edges, edge))
		return;
	edges->push_back(edge);
	recurseIn(edge->source(), edges);
}

void LambdaLifter::recurseIn(Node* node, std::vector<Edge*>* edges)
{
	if(node == 0)
		return;
	for(int i = 0; i < node->inArrity(); ++i)
		recurseIn(node->in(i), edges);
}


