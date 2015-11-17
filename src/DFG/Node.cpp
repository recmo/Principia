#include "Node.h"
#include <DFG/Edge.h>
#include <Parser/IdentifierProperty.h>
#include <Parser/SourceProperty.h>
#include <Utilities/assert.h>

std::shared_ptr<Node> Node::make_shared(NodeType type, int in, int out)
{
	std::shared_ptr<Node> node = std::make_shared<Node>(type, in, out);
	node->_self = node;
	node->createEdges();
	return std::move(node);
}

Node::Node(NodeType type, int incomingArity, int outgoingArity)
: PropertyMap()
, _type(type)
, _outgoing(outgoingArity)
, _incoming(incomingArity)
{
	assert(type.isValid());
	assert(inArity() >= (type == NodeType::Call) ? 1 : 0);
	assert(outArity() >= (type == NodeType::Closure)? 1 : 0);
}

void Node::createEdges()
{
	std::shared_ptr<Node> self = _self.lock();
	assert(self != nullptr);
	assert(self.get() == this);
	for(uint i = 0; i < outArity(); ++i)
		_outgoing[i] = std::move(std::make_shared<Edge>(self, i));
}

std::shared_ptr<Edge> Node::out(uint index) const
{
	if(index >= _outgoing.size())
		throw out_of_range(L"Index out of range.");
	std::shared_ptr<Edge> edge = _outgoing[index];
	assert(edge != nullptr);
	return edge;
}

std::shared_ptr<Edge> Node::in(uint index) const
{
	if(index >= _incoming.size())
		throw out_of_range(L"Index out of range.");
	return _incoming[index].lock();
}

void Node::connect(uint index, std::shared_ptr<Edge> edge)
{
	if(index >= _incoming.size())
		throw out_of_range(L"Index out of range.");
	
	// Switch the connection, updating Edge::_sinks in the process.
	const Edge::Sink sink{_self, index};
	std::weak_ptr<Edge>& in = _incoming[index];
	std::shared_ptr<Edge> old = in.lock();
	if(old != nullptr) {
		assert(contains(old->_sinks, sink));
		old->_sinks.erase(sink);
	}
	in = edge;
	if(edge != nullptr) {
		assert(!contains(edge->_sinks, sink));
		edge->_sinks.insert(sink);
	}
}

const vector<std::shared_ptr<Edge>>& Node::out() const
{
	return _outgoing;
}

vector<std::shared_ptr<Edge>> Node::in() const
{
	vector<std::shared_ptr<Edge>> result;
	result.reserve(_incoming.size());
	for(const auto& in: _incoming)
		result.push_back(in.lock());
	return result;
}

std::wostream& operator<<(std::wostream& out, const Node& node)
{
	if(node.type() == NodeType::Call)
		out << L"≔";
	if(node.has<IdentifierProperty>()) {
		out << node.get<IdentifierProperty>().value();
	} else if(node.has<SourceProperty>()) {
		SourceProperty sp = node.get<SourceProperty>();
		sp.print(out);
	} else {
		if(node.type() == NodeType::Closure
			&& node.out(0)->has<IdentifierProperty>()) {
			out << L"<" << node.out(0)->get<IdentifierProperty>().value() << L">";
		} else if(node.type() == NodeType::Call) {
			std::shared_ptr<Edge> in = node.in(0);
			if(in != nullptr && in->has<IdentifierProperty>()) {
				out << L"<" << in->get<IdentifierProperty>().value() << L">";
			} else {
				out << L"<anonymous>";
			}
		} else {
			out << L"<anonymous>";
		}
	}
	if(node.type() == NodeType::Closure)
		out << L"↦";
	return out;
}
