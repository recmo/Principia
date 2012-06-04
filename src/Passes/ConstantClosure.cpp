#include "ConstantClosure.h"
#include "ClosureProperty.h"
#include <DFG/DataFlowGraph.h>
#include <DFG/Node.h>
#include <Parser/ConstantProperty.h>
#include <Passes/Closure.h>

#define debug false

void ConstantClosure::anotateClosures()
{
	_fixedPoint = true;
	foreach(Node* node, _dfg->nodes()) {
		if(node->type() != NodeType::Closure)
			continue;
		if(!node->has<ClosureProperty>())
			continue;
		if(node->outArrity() < 1)
			continue;
		if(node->out(0)->has<ConstantProperty>())
			continue;
		if(node->get<ClosureProperty>().edges().empty()) {
			// We now have an closure node with an empty list of closures
			// Make it constant!
			node->out(0)->set(ConstantProperty(Value(new Closure(node))));
			if(debug)
				wcerr << node << " is constant " <<  node->out(0)->get<ConstantProperty>().value() << endl;
			
			_fixedPoint = false;
		} else {
			// Find all closure dependencies, recusively
			// If all of them are closure functions, then
			vector<Node*> seen;
			if(isRecursiveClosure(seen, node)) {
				if(debug)
					wcerr << node << " is recursively constant" << endl;
				node->set(ClosureProperty(vector<const Edge*>()));
				node->out(0)->set(ConstantProperty(Value(new Closure(node))));
				_fixedPoint = false;
			}
		}
	}
}

bool ConstantClosure::isRecursiveClosure(vector<Node*>& seen, Node* current)
{
	if(contains(seen, current))
		return true;
	if(!current->has<ClosureProperty>())
		return false;
	seen.push_back(current);
	foreach(const Edge* edge, current->get<ClosureProperty>().edges()) {
		Node* source = edge->source();
		if(!source)
			continue;
		if(source->type() != NodeType::Closure)
			return false;
		if(edge != source->out(0))
			return false;
		if(!isRecursiveClosure(seen, source))
			return false;
	}
	return true;
}

