#pragma once
#include <DFG/PropertyMap.h>
#include <DFG/InPort.h>
#include <DFG/OutPort.h>
#include <vector>
#include <memory>
#include <Utilities/not_null.h>

class Node: public PropertyMap, public std::enable_shared_from_this<Node>
{
public:
	enum NodeType {
		Call,
		Closure
	};
	Node(NodeType type, uint outgoingArity, uint incomingArity);
	
	NodeType type() const { return _type; }
	uint outArity() const { return _outgoing.size(); }
	uint inArity() const { return _incoming.size(); }
	
	OutPort& out(uint index);
	InPort& in(uint index);
	const OutPort& out(uint index) const;
	const InPort& in(uint index) const;
	
private:
	typedef not_null<std::shared_ptr<OutPort>> OutPortPtr;
	typedef not_null<std::shared_ptr<InPort>> InPortPtr;
	const NodeType _type;
	const std::vector<OutPortPtr> _outgoing;
	const std::vector<InPortPtr> _incoming;
	std::vector<OutPortPtr> createOutgoing(uint arity);
	std::vector<InPortPtr> createIncoming(uint arity);
};

std::wostream& operator<<(std::wostream& out, Node::NodeType nodeType);

std::wostream& operator<<(std::wostream& out, const Node& node);
