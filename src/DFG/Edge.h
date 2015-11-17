#pragma once
#include <DFG/PropertyMap.h>
class Node;

class Edge: public PropertyMap
{
public:
	struct Sink {
		std::weak_ptr<Node> target;
		uint index;
		bool operator==(const Sink& other) const;
		bool operator<(const Sink& other) const;
	};
	
	Edge() { }
	Edge(std::shared_ptr<Node> source, uint index);
	Edge(const Edge& copy) = delete;
	Edge& operator=(const Edge& copy) = delete;
	
	std::shared_ptr<Node> source() const;
	uint index() const { return _index; }
	const std::set<Sink>& sinks() const { return _sinks; }
	
	void replaceWith(std::shared_ptr<Edge> edge);
	bool isFunction() const;
	
private:
	friend class Node;
	const std::weak_ptr<Node> _source{};
	const uint _index = 0;
	std::set<Sink> _sinks;
};

std::wostream& operator<<(std::wostream& out, const Edge& edge);
