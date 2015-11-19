#include <Passes/ClosureCloser.h>
#include <Passes/ClosureProperty.h>
#include <DFG/DataFlowGraph.h>
#include <DFG/ConstantProperty.h>
#include <Utilities/containers.h>
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

void ClosureCloser::anotateClosures()
{
	_fixedPoint = true;
	for(auto node: _dfg.nodes()) {
		if(node->type() != Node::Closure)
			continue;
		anotateClosure(*node);
	}
}

void ClosureCloser::anotateClosure(Node& closureNode)
{
	assert(closureNode.type() == Node::Closure);
	
	// Calculate nodes between the closure's in an out ports.
	NodeSet past = causalPast(closureNode);
	NodeSet future = causalFuture(closureNode);
	NodeSet internal = past & future;
	wcerr << past << future << internal << endl;
	
	// Calculate the set of OutPorts linked from the internal nodes, but not
	// in the internal nodes.
	OutPortSet border;
	for(auto node: internal) {
		assert(node != nullptr);
		for(uint i = 0; i < node->inArity(); ++i) {
			InPort& in = node->in(i);
			if(in.source() == nullptr)
				continue;
			std::shared_ptr<OutPort> out = in.source();
			if(out == nullptr)
				continue;
			if(contains(internal, out->parent().shared_from_this()))
				continue;
			border.insert(out);
		}
	}
	
	wcerr << border << endl;
	
	// Apply arbitrary ordering
	ClosureProperty::ClosureSet vec;
	for(auto out: border)
		vec.push_back(out);
	
	// Set the closure property
	closureNode.set(ClosureProperty{vec});
	_fixedPoint = false;
}

ClosureCloser::NodeSet ClosureCloser::causalPast(Node& node)
{
	NodeSet result;
	causalPast(node, result);
	return result;
}

void ClosureCloser::causalPast(Node& node, ClosureCloser::NodeSet& past)
{
	past.insert(node.shared_from_this());
	for(uint i = 0; i < node.inArity(); ++i) {
		InPort& in = node.in(i);
		if(in.source() == nullptr)
			continue;
		auto parent = in.source()->parent().shared_from_this();
		if(contains(past, parent))
			continue;
		causalPast(*parent, past);
	}
}

ClosureCloser::NodeSet ClosureCloser::causalFuture(Node& node)
{
	NodeSet result;
	causalFuture(node, result);
	return result;
}

void ClosureCloser::causalFuture(Node& node, ClosureCloser::NodeSet& future)
{
	future.insert(node.shared_from_this());
	for(uint i = 0; i < node.outArity(); ++i) {
		OutPort& out = node.out(i);
		for(OutPort::Sink sink: out.sinks()) {
			if(std::shared_ptr<InPort> in = sink.lock()) {
				if(contains(future, in->parent().shared_from_this()))
					continue;
				causalFuture(in->parent(), future);
			}
		}
	}
}
