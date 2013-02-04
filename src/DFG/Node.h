#pragma once
#include "fixups.h"
#include <set>
#include "DFG/PropertyMap.h"
#include "DFG/NodeType.h"
#include "DFG/Edge.h"

class Node: public PropertyMap
{
public:
	Node(NodeType type, int inArrity, int outArity);
	~Node();
	
	NodeType type() const { return _type; }
	int inArrity() const { return _inArrity; }
	int outArrity() const { return _outArrity; }
	
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
	
	void check() const;
	
protected:
	NodeType _type;
	uint _inArrity;
	uint _outArrity;
	Edge** _incomming;
	Edge*  _outgoing;
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
	assert(index < _outArrity);
	return _outgoing + index;
}

inline const Edge* Node::out(uint index) const
{
	assert(index < _outArrity);
	return _outgoing + index;
}

inline Edge* Node::in(uint index)
{
	assert(index < _inArrity);
	return _incomming[index];
}

inline const Edge* Node::in(uint index) const
{
	assert(index < _inArrity);
	return _incomming[index];
}

