#pragma once
#include <DFG/PropertyMap.h>
#include <DFG/Node.h>
#include <set>
#include <memory>
#include <Utilities/not_null.h>

class DataFlowGraph: public PropertyMap
{
public:
	Node& make_node(Node::Type type, uint out, uint in);
	Node& make_call(uint out, uint in)
	{ return make_node(Node::Call, out, in); }
	Node& make_closure(uint out, uint in)
	{ return make_node(Node::Closure, out, in); }
	
	void addNode(std::shared_ptr<Node> node);
	const std::set<std::shared_ptr<Node>>& nodes() const { return _nodes; }
	
private:
	std::set<std::shared_ptr<Node>> _nodes;
};
