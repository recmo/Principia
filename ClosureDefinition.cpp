#include "ClosureDefinition.h"
#include "Closure.h"
#include "Symbol.h"
#include "Context.h"

Closure* ClosureDefinition::close(const Context* context) const
{
	// Copy the context
	Context* my_context = new Context;
	for(auto it = context->values.begin(); it != context->values.end(); ++it)
	{
		my_context->values.insert(*it);
	}
	
	// Create a closure using this context
	Closure* closure = new Closure(this, my_context);
	
	// Add the current closure to the context to allow recursion
	my_context->values[function] = Value(closure);
	
	// Return the closure
	return closure;
}

std::wostream& operator<<(std::wostream& out, const ClosureDefinition* cd)
{
	if(cd == 0)
	{
		out << L"null";
	}
	else
	{
		out << cd->function << cd->arguments << L" ↦ " << cd->returns;
	}
	return out;
}
