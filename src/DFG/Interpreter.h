#pragma once
#include <DFG/Node.h>
#include <DFG/Value.h>
#include <map>
#include <memory>

class Interpreter
{
public:
	typedef std::vector<Value> Values;
	
	Values evaluate(const Node& closure, const Values& arguments);
	
private:
	typedef std::shared_ptr<const OutPort> OutPortPtr;
	typedef std::map<OutPortPtr, Value> Context;
};
