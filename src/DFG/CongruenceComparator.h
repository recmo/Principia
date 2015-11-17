#pragma once
#include <vector>
#include <map>
#include <memory>
class Edge;
class Node;
class Value;

/// Compare parts of the data flow graph for congruence
/// This captures the notion structural equivalence, isomorphism, intensional
/// equivalence, etc..
class CongruenceComparator
{
public:
	typedef std::map<std::shared_ptr<Edge>, std::shared_ptr<Edge>> Connections;
	
	void connectLeft(const Connections& connections) { _connectLeft = connections; }
	void connectRight(const Connections& connections) { _connectRight = connections; }
	
	bool compareEdges(std::shared_ptr<Edge> left, std::shared_ptr<Edge> right);
	bool compareNodes(std::shared_ptr<Node> left, std::shared_ptr<Node> right);
	bool compareValues(const Value& left, const Value& right);
	
protected:
	Connections _connectLeft;
	Connections _connectRight;
	typedef std::pair<std::shared_ptr<Edge>, std::shared_ptr<Edge>> EdgePair;
	std::vector<EdgePair> _recursionPairs;
};
