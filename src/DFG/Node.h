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
	
	const Edge* in(int index) const;
	Edge* in(int index);
	const Edge* out(int index) const;
	Edge* out(int index);
	
	void connect(int index, Edge* edge);
	void forgetEdge(const Edge* edge);
	void replaceEdge(const Edge* from, Edge* to);
	
	void print(std::wostream& out) const;
	
	std::set<Node*> outNodes(bool ignoreFunctional);
	std::set<Node*> outClosures(bool ignoreFunctional);
	
protected:
	NodeType _type;
	int _inArrity;
	int _outArrity;
	Edge** _incomming;
	Edge*  _outgoing;
};

inline std::wostream& operator<<(std::wostream& out, const Node& node)
{
	node.print(out);
	return out;
}

inline Edge* Node::out(int index)
{
	assert(index >= 0 && index < _outArrity);
	return _outgoing + index;
}

inline const Edge* Node::out(int index) const
{
	assert(index >= 0 && index < _outArrity);
	return _outgoing + index;
}

inline Edge* Node::in(int index)
{
	assert(index >= 0 && index < _inArrity);
	return _incomming[index];
}

inline const Edge* Node::in(int index) const
{
	assert(index >= 0 && index < _inArrity);
	return _incomming[index];
}

