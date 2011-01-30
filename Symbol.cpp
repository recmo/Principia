#include "fixups.h"
#include "Symbol.h"
#include "ClosureDefinition.h"
#include "CallDefinition.h"
#include "Closure.h"
#include "Builtins.h"

Symbol::Symbol()
: closure(0), call(0)
{

}

Value Symbol::evaluate(Context& context)
{
	// Return value if already defined in this context
	map<Symbol*, Value>::const_iterator it;
	it = context.find(this);
	if(it != context.end())
	{
		return it->second;
	}
	if(call)
	{
		// wcerr << L"↦" << call->function->label << endl;
		
		// Pack the arguments
		vector<Value> arguments;
		arguments.reserve(call->arguments.size());
		vector<Symbol*>::const_iterator arg;
		for(arg = call->arguments.begin(); arg != call->arguments.end(); ++arg)
		{
			arguments.push_back((*arg)->evaluate(context));
		}
		
		vector<Value> returns;
		auto builtin = builtins.find(call->function->label);
		if(builtin != builtins.end())
		{
			// Function is builtin
			returns = (builtin->second)(arguments);
		}
		else
		{
			// This symbol is defined as a return value
			Value closure_value = call->function->evaluate(context);
			
			// Call the closure
			const Closure* c = closure_value.function();
			returns = c->evaluate(arguments);
		}
		
		// Unpack the return values
		// assert(returns.size() == call->returns.size());
		vector<Symbol*>::const_iterator symbol =  call->returns.begin();
		vector<Value>::const_iterator value = returns.begin();
		for(; symbol != call->returns.end(); ++symbol, ++value)
		{
			context[*symbol] = *value;
		}
		
		// Return our value
		return context[this];
	}
	if(closure)
	{
		// wcerr << L"≔" << closure->function->label << endl;
		
		// This symbol is defined as a the result of a closure
		Closure* c =  new Closure(closure, context);
		Value value = Value(c);
		context[this] = value;
		return value;
	}
	try
	{
		/// TODO: double types and string types
		return Value(parse<sint64>(label));
	}
	catch(...)
	{
		throw std::runtime_error("Could not evaluate symbol");
	}
}

std::wostream& operator<<(std::wostream& out, const Symbol* s)
{
	out << L"<" << s->label << L">";
	return out;
}
