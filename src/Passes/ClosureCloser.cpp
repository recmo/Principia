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
	for(auto node: _dfg.nodes()) {
		if(node->type() != NodeType::Closure)
			continue;
		anotateClosure(node);
	}
}

void ClosureCloser::anotateClosure(std::shared_ptr<Node> closureNode)
{
	assert(closureNode->type() == NodeType::Closure);
	
	// Calculate internal nodes
	vector<std::shared_ptr<Node>> internalNodes;
	internalNodes.push_back(closureNode);
	for(auto edge: closureNode->out()) {
		for(auto sink: edge->sinks())
			recurseOut(sink.target.lock(), &internalNodes);
	}
	recurseOut(closureNode, &internalNodes);
	
	if(debug)
		wcerr << closureNode << " internalNodes " << internalNodes << endl;
	
	// Calculate the lazy set
	bool fixedpoint = false;
	vector<std::shared_ptr<Node>> lazySet = internalNodes;
	while(!fixedpoint) {
		fixedpoint = true;
		
		// Calculate the direct source nodes
		vector<std::shared_ptr<Node>> directSources;
		for(std::shared_ptr<Node> lazy: lazySet) {
			for(const std::shared_ptr<Edge> edge: lazy->in()) {
				if(!edge->source() && edge->has<ConstantProperty>())
					continue;
				std::shared_ptr<Node> directSource = edge->source();
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
		for(std::shared_ptr<Node> direct: directSources) {
			
			if(debug)
				wcerr << "Considering adding " << direct << " to the lazy list." << endl;
			
			// Abort if a sink is outside the lazy set for a closure node
			/// @todo As many call nodes as possible are included, this is not at all lazy,
			/// but we must try to create closures that are minimal
			bool abort = false;
			for(auto out: direct->out()) {
				for(auto sink: out->sinks()) {
					auto sinkNode = sink.target.lock();
					if(!contains(lazySet, sinkNode)) {
						if(debug) {
							wcerr << "Result escapes from: ";
							wcerr << direct << " through " << out;
							wcerr << " into " << sinkNode << endl;
						}
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
	vector<std::shared_ptr<Edge>> border;
	for(auto lazyNode: lazySet) {
		for(auto in: lazyNode->in()) {
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
		const auto& oldList = closureNode->get<ClosureProperty>().edges();
		
		// Check if they are identical
		if(oldList.size() == border.size()) {
			bool same = true;
			for(auto edge: oldList) {
				if(!contains(border, edge)) {
					same = false;
					break;
				}
			}
			
			// Return without changing anything
			if(same)
				return;
		}
	}
	
	// Set the closure property
	ClosureProperty cp(border);
	closureNode->set(cp);
	_fixedPoint = false;
}

void ClosureCloser::recurseOut(std::shared_ptr<Edge> edge, vector<std::shared_ptr<Edge>>* edges)
{
	if(contains(*edges, edge))
		return;
	edges->push_back(edge);
	for(auto sink: edge->sinks())
		recurseOut(sink.target.lock(), edges);
}

void ClosureCloser::recurseOut(std::shared_ptr<Node> node, vector<std::shared_ptr<Edge>>* edges)
{
	for(auto out: node->out())
		recurseOut(out, edges);
}

void ClosureCloser::recurseOut(std::shared_ptr<Node> node, vector<std::shared_ptr<Node>>* nodes)
{
	if(contains(*nodes, node))
		return;
	nodes->push_back(node);
	for(auto out: node->out()) {
		for(auto sink: out->sinks())
			recurseOut(sink.target.lock(), nodes);
	}
}

void ClosureCloser::recurseIn(std::shared_ptr<Edge> edge, std::vector<std::shared_ptr<Edge>>* edges)
{
	if(contains(*edges, edge))
		return;
	edges->push_back(edge);
	recurseIn(edge->source(), edges);
}

void ClosureCloser::recurseIn(std::shared_ptr<Node> node, std::vector<std::shared_ptr<Edge>>* edges)
{
	if(!node || !edges)
		return;
	for(uint i = 0; i < node->inArity(); ++i)
		recurseIn(node->in(i), edges);
}
