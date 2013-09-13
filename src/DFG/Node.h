#pragma once
#include "fixups.h"
#include <limits>
#include <set>
#include "DFG/PropertyMap.h"
#include "DFG/NodeType.h"
#include "DFG/Edge.h"

class Node: public PropertyMap
{
public:
	static const uint noIndex = std::numeric_limits<uint>::max();
	
	Node(NodeType type, int incommingArity, int outgoingArity);
	~Node();
	
	NodeType type() const { return _type; }
	uint inArity() const { return _incomming.size(); }
	uint outArity() const { return _outgoing.size(); }
	
	const Edge* in(uint index) const;
	Edge* in(uint index);
	std::vector<const Edge*> constIn() const;
	std::vector<Edge*> in();
	uint inIndexOf(const Edge* edge) const;
	
	const Edge* out(uint index) const;
	Edge* out(uint index);
	std::vector<const Edge*> constOut() const;
	std::vector<Edge*> out();
	uint outIndexOf(const Edge* edge) const;
	
	void connect(uint index, Edge* edge);
	void forgetEdge(const Edge* edge);
	void replaceEdge(const Edge* from, Edge* to);
	
	void print(std::wostream& out) const;
	
	std::set<Node*> outNodes(bool ignoreFunctional);
	std::set<Node*> outClosures(bool ignoreFunctional);
	
protected:
	NodeType _type;
	std::vector<Edge*> _incomming;
	std::vector<Edge*> _outgoing;
};

inline std::wostream& operator<<(std::wostream& out, const Node& node)
{
	node.print(out);
	return out;
}

inline std::wostream& operator<<(std::wostream& out, const Node* node)
{
	return (node) ? out << *node : out << L"nullNode";
}

inline Edge* Node::out(uint index)
{
	assert(index < outArity());
	return _outgoing[index];
}

inline const Edge* Node::out(uint index) const
{
	assert(index < outArity());
	return _outgoing[index];
}

inline Edge* Node::in(uint index)
{
	assert(index < inArity());
	return _incomming[index];
}

inline const Edge* Node::in(uint index) const
{
	assert(index < inArity());
	return _incomming[index];
}

