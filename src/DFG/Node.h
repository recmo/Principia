#pragma once
#include <DFG/PropertyMap.h>
#include <DFG/NodeType.h>
#include <vector>
#include <memory>
class Edge;

class Node: public PropertyMap
{
public:
	static std::shared_ptr<Node> make_shared(NodeType type,
		int incomingArity, int outgoingArity);
		
	Node(NodeType type, int incomingArity, int outgoingArity);
	
	NodeType type() const { return _type; }
	uint inArity() const { return _incoming.size(); }
	uint outArity() const { return _outgoing.size(); }
	
	std::shared_ptr<Edge> out(uint index) const;
	std::shared_ptr<Edge> in(uint index) const;
	void connect(uint index, std::shared_ptr<Edge> edge);
	
	const std::vector<std::shared_ptr<Edge>>& out() const;
	std::vector<std::shared_ptr<Edge>> in() const;
	
private:
	const NodeType _type;
	std::weak_ptr<Node> _self;
	std::vector<std::shared_ptr<Edge>> _outgoing;
	std::vector<std::weak_ptr<Edge>> _incoming;
	
	void createEdges();
};

std::wostream& operator<<(std::wostream& out, const Node& node);
