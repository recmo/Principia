#include "Passes/ClosureCloser.h"
#include <DFG/DataFlowGraph.h>
#include <DFG/Node.h>
#include <DFG/Edge.h>
#include <Passes/ClosureProperty.h>
#include <Parser/SourceProperty.h>
#include <Parser/IdentifierProperty.h>
#include <Parser/ConstantProperty.h>

#define debug false

void ClosureCloser::anotateClosures()
{
	_fixedPoint = true;
	foreach(Node* node, _dfg->nodes()) {
		if(node->type() != NodeType::Closure)
			continue;
		anotateClosure(node);
	}
}

void ClosureCloser::anotateClosure(Node* closureNode)
{
	assert(closureNode->type() == NodeType::Closure);
	
	// Calculate internal nodes
	vector<Node*> internalNodes;
	internalNodes.push_back(closureNode);
	for(uint i = 1; i < closureNode->outArity(); ++i) {
		foreach(Node* sink, closureNode->out(i)->sinks())
			recurseOut(sink, &internalNodes);
	}
	recurseOut(closureNode, &internalNodes);
	
	if(debug)
		wcerr << closureNode << " internalNodes " << internalNodes << endl;
	
	// Calculate the lazy set
	bool fixedpoint = false;
	vector<Node*> lazySet = internalNodes;
	while(!fixedpoint) {
		fixedpoint = true;
		
		// Calculate the direct source nodes
		vector<Node*> directSources;
		foreach(Node* lazy, lazySet) {
			foreach(const Edge* edge, lazy->in()) {
				if(!edge->source() && edge->has<ConstantProperty>())
					continue;
				Node* directSource = edge->source();
				if(!directSource)
					wcerr << edge << " has no source" << endl;
				assert(directSource);
				if(directSource->type() == NodeType::Closure && edge != directSource->out(0))
					continue; // Only make-closure is valid
				if(contains(lazySet, directSource))
					continue;
				if(contains(directSources, directSource))
					continue;
				directSources.push_back(edge->source());
			}
		}
		
		// For each direct source node
		foreach(Node* direct, directSources) {
			
			if(debug)
				wcerr << "Considering adding " << direct << " to the lazy list." << endl;
			
			// Abort if a sink is outside the lazy set for a closure node
			/// @todo As many call nodes as possible are included, this is not at all lazy,
			/// but we must try to create closures that are minimal
			bool abort = false;
			foreach(const Edge* sink, direct->out()) {
				foreach(Node* sinkNode, sink->sinks()) {
					if(!contains(lazySet, sinkNode)) {
						if(debug)
							wcerr << "Result escapes from: " << direct << " through " << sink << " into " << sinkNode << endl;
						abort = direct->type() == NodeType::Closure;
						break;
					}
				}
				if(abort)
					break;
			}
			if(abort)
				continue;
			
			// Add to the lazy set
			assert(!contains(lazySet, direct));
			lazySet.push_back(direct);
			fixedpoint = false;
		}
	}
	if(debug)
		wcerr << closureNode << " lazy set " << lazySet << endl;
	
	// Calculate the border
	vector<const Edge*> border;
	foreach(Node* lazyNode, lazySet) {
		foreach(const Edge* in, lazyNode->in()) {
			if(in->has<ConstantProperty>())
				continue;
			assert(in->source());
			if(contains(lazySet, in->source()))
				continue;
			if(contains(border, in))
				continue;
			border.push_back(in);
		}
	}
	if(debug)
		wcerr << closureNode << " border " << border << endl;
	
	// Check if it already has a closure list
	if(closureNode->has<ClosureProperty>()) {
		const vector<const Edge*>& oldList = closureNode->get<ClosureProperty>().edges();
		
		// Check if they are identical
		if(oldList.size() == border.size()) {
			bool same = true;
			foreach(const Edge* edge, oldList) {
				if(!contains(border, edge)) {
					same = false;
					break;
				}
			}
			
			// Return without chaning anything
			if(same)
				return;
		}
	}
	
	// Set the closure property
	ClosureProperty cp(border);
	closureNode->set(cp);
	_fixedPoint = false;
}

void ClosureCloser::recurseOut(Edge* edge, vector<Edge*>* edges)
{
	if(contains(*edges, edge))
		return;
	edges->push_back(edge);
	foreach(Node* node, edge->sinks())
		recurseOut(node, edges);
}

void ClosureCloser::recurseOut(Node* node, vector<Edge*>* edges)
{
	for(uint i = 0; i < node->outArity(); ++i)
		recurseOut(node->out(i), edges);
}

void ClosureCloser::recurseOut(Node* node, vector<Node*>* nodes)
{
	if(contains(*nodes, node))
		return;
	nodes->push_back(node);
	for(uint i = 0; i < node->outArity(); ++i) {
		foreach(Node* sink, node->out(i)->sinks())
			recurseOut(sink, nodes);
	}
}

void ClosureCloser::recurseIn(Edge* edge, std::vector<Edge*>* edges)
{
	if(contains(*edges, edge))
		return;
	edges->push_back(edge);
	recurseIn(edge->source(), edges);
}

void ClosureCloser::recurseIn(Node* node, std::vector<Edge*>* edges)
{
	if(!node || !edges)
		return;
	for(uint i = 0; i < node->inArity(); ++i)
		recurseIn(node->in(i), edges);
}
