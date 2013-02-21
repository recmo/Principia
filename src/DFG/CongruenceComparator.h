#pragma once
#include <vector>
#include <map>
class Edge;
class Node;
class Value;

/// Compare parts of the data flow graph for congruence
/// This captures the notion structural equivalence, isomorphism, intensional equivalence, etc..
class CongruenceComparator
{
public:
	typedef std::map<const Edge*, const Edge*> Connections;
	CongruenceComparator();
	~CongruenceComparator();
	
	void connectLeft(const Connections& connections) { _connectLeft = connections; }
	void connectRight(const Connections& connections) { _connectRight = connections; }
	
	bool compareEdges(const Edge* left, const Edge* right);
	bool compareNodes(const Node* left, const Node* right);
	bool compareValues(const Value& left, const Value& right);
	
protected:
	Connections _connectLeft;
	Connections _connectRight;
	std::vector<std::pair<const Edge*, const Edge*>> _recursionPairs;
};

