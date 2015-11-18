#include "Node.h"
#include <DFG/OutPort.h>
#include <DFG/InPort.h>
#include <Parser/IdentifierProperty.h>
#include <Parser/SourceProperty.h>
#include <Utilities/assert.h>
#include <Utilities/exceptions.h>
#include <Unicode/exceptions.h>
#include <Unicode/convert.h>

Node::Node(Type type, uint outgoingArity, uint incomingArity)
: PropertyMap()
, _type(type)
, _outgoing(createOutgoing(outgoingArity))
, _incoming(createIncoming(incomingArity))
{
	assert(type == Call || type == Closure);
	assert(outgoingArity >= (type == Node::Closure)? 1 : 0);
	assert(incomingArity >= (type == Node::Call) ? 1 : 0);
}

std::vector<Node::OutPortPtr> Node::createOutgoing(uint arity)
{
	std::vector<OutPortPtr> result;
	result.reserve(arity);
	for(uint i = 0; i < arity; ++i)
		result.push_back(std::make_shared<OutPort>(*this, i));
	assert(result.size() == arity);
	return result;
}

std::vector<Node::InPortPtr> Node::createIncoming(uint arity)
{
	std::vector<InPortPtr> result;
	result.reserve(arity);
	for(uint i = 0; i < arity; ++i)
		result.push_back(std::make_shared<InPort>(*this, i));
	assert(result.size() == arity);
	return result;
}

OutPort& Node::out(uint index)
{
	if(index >= _outgoing.size())
		throw index_out_of_range{};
	OutPort* const ptr = _outgoing.at(index).get().get();
	return *ptr;
}

const OutPort& Node::out(uint index) const
{
	if(index >= _outgoing.size())
		throw index_out_of_range{};
	const OutPort* const ptr = _outgoing.at(index).get().get();
	return *ptr;
}

InPort& Node::in(uint index)
{
	if(index >= _incoming.size())
		throw index_out_of_range{};
	InPort* const ptr = _incoming.at(index).get().get();
	return *ptr;
}

const InPort& Node::in(uint index) const
{
	if(index >= _incoming.size())
		throw index_out_of_range{};
	const InPort* const ptr = _incoming.at(index).get().get();
	return *ptr;
}

std::wostream& operator<<(std::wostream& out, Node::Type nodeType)
{
	switch(nodeType) {
		case Node::Call: return out << L"Call";
		case Node::Closure: return out << L"Closure";
		default: throw invalid_enum{};
	};
}

std::wostream& operator<<(std::wostream& out, const Node& node)
{
	if(node.type() == Node::Call)
		out << L"≔";
	if(node.has<IdentifierProperty>()) {
		out << node.get<IdentifierProperty>();
	} else if(node.has<SourceProperty>()) {
		out << node.get<SourceProperty>();
	} else {
		if(node.type() == Node::Closure) {
			const OutPort& outp = node.out(0);
			if(outp.has<IdentifierProperty>()) {
				out << L"<" << outp.get<IdentifierProperty>() << L">";
			} else {
				out << L"<anonymous>";
			}
		} else if(node.type() == Node::Call) {
			const InPort& inp = node.in(0);
			if(inp.has<IdentifierProperty>()) {
				out << L"<" << inp.get<IdentifierProperty>() << L">";
			} else {
				out << L"<anonymous>";
			}
		}
	}
	if(node.type() == Node::Closure)
		out << L"↦";
	return out;
}

std::ostream& operator<<(std::ostream& out, const Node& node)
{
	return out << encodeLocal(toString(node));
}
