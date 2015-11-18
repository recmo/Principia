#include "Interpreter.h"
#include <Utilities/exceptions.h>
#include <DFG/ConstantProperty.h>

Interpreter::Values Interpreter::evaluate(const Closure& closure,
	const Interpreter::Values& arguments)
{
	assert(closure.node != nullptr);
	assert(closure.node->type() == Node::Closure);
	return evaluate(*(closure.node), arguments, closure.context);
}

Interpreter::Values Interpreter::evaluate(const Node& node,
	Interpreter::Context context)
{
	// Evaluate in ports
	Values arguments;
	arguments.reserve(node.inArity());
	for(uint i = 1; i < node.inArity(); ++i)
		arguments.push_back(evaluate(node.in(i), context));
	
	// Evaluate node
	return evaluate(node, arguments, context);
}

Interpreter::Values Interpreter::evaluate(const Node& node,
	const Interpreter::Values& arguments, Interpreter::Context context)
{
	return node.type() == Node::Call
		? evaluateCall(node, arguments, context)
		: evaluateClosure(node, arguments, context);
}

Value Interpreter::evaluate(const InPort& in, Interpreter::Context context)
{
	// Constants
	if(in.has<ConstantProperty>())
		return in.get<ConstantProperty>().value();
	
	// Sourced
	OutPortPtr source = in.source();
	assert(source != nullptr);
	return evaluate(*source, context);
}

Value Interpreter::evaluate(const OutPort& out, Interpreter::Context context)
{
	// Context
	if(contains(context, out.shared_from_this()))
		return context[out.shared_from_this()];
	
	// Create closure on first closure output.
	if(out.parent().type() == Node::Closure && out.index() == 0)
		return Value{Closure{out.parent().shared_from_this(), context}};
	
	// Evaluate parent statement and pick the right output.
	return evaluate(out.parent(), context)[out.index()];
}

Interpreter::Values Interpreter::evaluateCall(const Node& call,
	const Interpreter::Values& arguments, Interpreter::Context context)
{
	assert(call.type() == Node::Call);
	assert(call.inArity() == arguments.size() + 1);
	
	// Fetch the closure
	Value func = evaluate(call.in(0), context);
	assert(func.type() == Value::Closure || func.type() == Value::ExtFunc);
	
	// Call closures
	if(func.type() == Value::Closure) {
		const Values result = evaluate(func.closure(), arguments);
		assert(result.size() == call.outArity());
		return result;
	}
	
	// Call external functions
	if(func.type() == Value::ExtFunc) {
		const Values result = func.func()(arguments);
		assert(result.size() == call.outArity());
		return result;
	}
	
	throw unreachable{};
}

Interpreter::Values Interpreter::evaluateClosure(const Node& closure,
	const Interpreter::Values& arguments, Interpreter::Context context)
{
	assert(closure.type() == Node::Closure);
	assert(closure.outArity() == arguments.size() + 1);
	
	// Set the current closure to the first output to allow recursion.
	context[closure.out(0).shared_from_this()] = 
		Value{Closure{closure.shared_from_this(), context}};
	
	// Set the other outputs to the arguments.
	for(uint i = 1; i < closure.outArity(); ++i)
		context[closure.out(i).shared_from_this()] = arguments[i - 1];
	
	// Pull the inputs
	Values result;
	result.reserve(closure.inArity());
	for(uint i = 0; i < closure.inArity(); ++i)
		result.push_back(evaluate(closure.in(i), context));
	assert(result.size() == closure.inArity());
	return result;
}
