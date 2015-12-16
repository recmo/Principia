#include <Passes/ClosureCloser.h>
#include <Passes/ClosureProperty.h>
#include <DFG/DataFlowGraph.h>
#include <DFG/ConstantProperty.h>
#include <Utilities/containers.h>
#include <Utilities/exceptions.h>
#include <iostream>
using std::wcerr;
using std::endl;

#define debug false

// internal nodes are call or closure nodes
// that are required for the return values of the closure
// and depend on the inputs to the closure. This forces
// them to be computed every time the closure is evaluated,

// If the node is a closure node it's internal and lazy
// nodes are not included, even if the results depend on a
// call to the resulting closure.

// lazy nodes are call or closure nodes that are required
// for the return values of this closure and only for the
// return values of this closure without depending on the
// input values. That is, they need to be evaluated exactly
// once before the closure can be evaluated.

void ClosureCloser::annotateClosures()
{
	for(auto node: _dfg.nodes()) {
		if(node->type() != Node::Closure)
			continue;
		annotateClosure(*node);
	}
}

void ClosureCloser::annotateClosure(Node& closureNode)
{
	assert(closureNode.type() == Node::Closure);
	if(closureNode.has<ClosureProperty>())
		return;
	
	// Calculate the body of the closure
	NodeSet body = closureBody(closureNode);
	
	// Calculate the set of OutPorts linked from the body nodes, but not
	// in the body node set.
	OutPortSet border;
	for(auto node: body) {
		assert(node != nullptr);
		for(uint i = 0; i < node->inArity(); ++i) {
			InPort& in = node->in(i);
			if(in.source() == nullptr)
				continue;
			std::shared_ptr<OutPort> out = in.source();
			if(out == nullptr)
				continue;
			if(contains(body, out->parent().shared_from_this()))
				continue;
			border.insert(out);
		}
	}
	
	// Add any OutPorts linked directly to the InPorts, but not in the body set.
	for(uint i = 0; i < closureNode.inArity(); ++i) {
		InPort& in = closureNode.in(i);
		if(in.source() == nullptr)
			continue;
		std::shared_ptr<OutPort> out = in.source();
		if(out == nullptr)
			continue;
		if(contains(body, out->parent().shared_from_this()))
			continue;
		border.insert(out);
	}
	
	// Note: there are more meaningful sets other than `body`:
	//
	// * `lazy`: The nodes that feed only into body, but are not in body. These
	//           nodes can be deferred for lazy evaluation, turning a *lazy*
	//           closure context into a non-lazy one.
	// * `dead`: Nodes part of body, but who's output is does not end up in the
	//           input of the closure. These can be eliminated without
	//           consequences.
	
	// Apply arbitrary ordering
	ClosureProperty::ClosureSet vec;
	for(auto out: border)
		vec.push_back(out);
	
	// Set the closure property
	closureNode.set(ClosureProperty{vec});
}

ClosureCloser::NodeSet ClosureCloser::closureBody(Node& closure)
{
	// Add all the nodes causally after the closure outputs (skipping the first)
	assert(closure.type() == Node::Closure);
	NodeSet result;
	for(uint i = 1; i < closure.outArity(); ++i)
		closureBody(closure.out(i), result);
	return result;
}

void ClosureCloser::closureBody(OutPort& out, ClosureCloser::NodeSet& set)
{
	for(auto wsink: out.sinks())
		if(auto sink = wsink.lock())
			closureBody(*sink, set);
}

void ClosureCloser::closureBody(InPort& in, ClosureCloser::NodeSet& set)
{
	closureBody(in.parent(), set);
}

void ClosureCloser::closureBody(Node& node, ClosureCloser::NodeSet& set)
{
	auto ptr = node.shared_from_this();
	if(contains(set, ptr))
		return;
	set.insert(ptr);
	for(uint i = 0; i < node.outArity(); ++i)
		closureBody(node.out(i), set);
}
