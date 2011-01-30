#include "Closure.h"
#include "ClosureDefinition.h"
#include "Context.h"
#include "Symbol.h"

Closure::Closure(const ClosureDefinition* clo, const Context* con)
: closure(clo), context(con)
{
}

vector<Value> Closure::evaluate(const std::vector<Value>& arguments) const
{
	// Create an execution context
	Context exec;
	
	// Start with the closed over context
	for(auto it = context->values.begin(); it != context->values.end(); ++it)
	{
		exec.values.insert(*it);
	}
	
	// Add the arguments
	assert(arguments.size() == closure->arguments.size());
	auto sit = closure->arguments.begin();
	auto vit = arguments.begin();
	for(; sit != closure->arguments.end(); ++sit, ++ vit)
	{
		exec.values[*sit] = *vit;
	}
	
	// Evaluate the return values
	vector<Value> returns;
	for(sit = closure->returns.begin(); sit != closure->returns.end(); ++sit)
	{
		returns.push_back((*sit)->evaluate(&exec));
	}
	
	return returns;
}

