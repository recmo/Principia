#pragma once
#include <vector>
class Edge;
class Node;
class Value;

/// Compare parts of the data flow graph for congruence
/// This captures the notion structural equivalence, isomorphism, intensional equivalence, etc..
class CongruenceComparator
{
public:
	static bool compare(const Edge* left, const Edge* right);
	static bool compare(const Node* left, const Node* right);
	static bool compare(const Value& left, const Value& right);
	
protected:
	CongruenceComparator();
	~CongruenceComparator();
	bool compareEdges(const Edge* left, const Edge* right);
	bool compareNodes(const Node* left, const Node* right);
	bool compareValues(const Value& left, const Value& right);
	std::vector<std::pair<const Edge*, const Edge*>> recursionPairs;
};

