#pragma once
#include <DFG/Node.h>
class Value;

struct Closure {
	typedef std::shared_ptr<const OutPort> OutPortPtr;
	typedef std::map<OutPortPtr, Value> Context;
	std::shared_ptr<const Node> node;
	Context context;
};
