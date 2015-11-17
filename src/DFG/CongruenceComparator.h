#pragma once
#include <DFG/Node.h>
#include <vector>
#include <map>
#include <memory>
class Node;
class Value;

/// Compare parts of the data flow graph for congruence
/// This captures the notion structural equivalence, isomorphism, intensional
/// equivalence, etc..
class CongruenceComparator
{
public:
	typedef std::map<std::shared_ptr<OutPort>,
		std::shared_ptr<OutPort>> Connections;
	
	void connectLeft(const Connections& connections) { _connectLeft = connections; }
	void connectRight(const Connections& connections) { _connectRight = connections; }
	
	bool compareNode(const Node& left, const Node& right);
	bool compareIn(const InPort& left, const InPort& right);
	bool compareOut(const OutPort& left, const OutPort& right);
	bool compareValues(const Value& left, const Value& right);
	
protected:
	Connections _connectLeft;
	Connections _connectRight;
	typedef std::pair<std::shared_ptr<const OutPort>,
		std::shared_ptr<const OutPort>> OutPair;
	std::vector<OutPair> _recursionPairs;
};
