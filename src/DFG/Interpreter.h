#pragma once
#include <DFG/Node.h>
#include <DFG/Value.h>
#include <map>
#include <memory>

class Interpreter
{
public:
	typedef std::vector<Value> Values;
	typedef std::shared_ptr<const OutPort> OutPortPtr;
	typedef std::map<OutPortPtr, Value> Context;
	
	static Values evaluate(const Closure& closure, const Values& arguments);
	static Values evaluate(const Node& node, const Values& arguments,
		Context context = Context());
	static Values evaluate(const Node& node, Context context = Context());
	static Value evaluate(const OutPort& out, Context context = Context());
	static Value evaluate(const InPort& in, Context context = Context());
	
private:
	static Values evaluateCall(const Node& call, const Values& arguments,
		Context context = Context());
	static Values evaluateClosure(const Node& closure, const Values& arguments,
		Context context = Context());
};
