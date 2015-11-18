#include "Interpreter.h"
#include <Utilities/exceptions.h>

Interpreter::Values Interpreter::evaluate(const Node& closure,
	const Interpreter::Values& arguments)
{
	assert(closure.type() == Node::Closure);
	assert(closure.outArity() == arguments.size() + 1);
	
	// Set the outputs
	Context body;
	// body[closure.out(0)] = ;
	for(uint i = 1; i < closure.outArity(); ++i)
		body[closure.out(i).shared_from_this()] = arguments[i - 1];
	
	// Pull the inputs
	
	
	throw unimplemented{};
}
