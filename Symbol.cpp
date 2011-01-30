#include "fixups.h"
#include "Symbol.h"
#include "ClosureDefinition.h"
#include "CallDefinition.h"
#include "Context.h"
#include "Closure.h"
#include "Builtins.h"

Symbol::Symbol()
: closure(0), call(0)
{

}

uint64 Symbol::evaluate(Context* context)
{
	// Return value if already defined in this context
	map<Symbol*, uint64>::const_iterator it;
	it = context->values.find(this);
	if(it != context->values.end())
	{
		return it->second;
	}
	if(call)
	{
		// wcerr << L"↦" << call->function->label << endl;
		
		// Pack the arguments
		vector<uint64> arguments;
		arguments.reserve(call->arguments.size());
		vector<Symbol*>::const_iterator arg;
		for(arg = call->arguments.begin(); arg != call->arguments.end(); ++arg)
		{
			arguments.push_back((*arg)->evaluate(context));
		}
		
		vector<uint64> returns;
		auto builtin = builtins.find(call->function->label);
		if(builtin != builtins.end())
		{
			// Function is builtin
			returns = (builtin->second)(arguments);
		}
		else
		{
			// This symbol is defined as a return value
			uint64 closure_value = call->function->evaluate(context);
			
			// Call the closure
			Closure* c = reinterpret_cast<Closure*>(closure_value);
			returns = c->evaluate(arguments);
		}
		
		// Unpack the return values
		// assert(returns.size() == call->returns.size());
		vector<Symbol*>::const_iterator symbol =  call->returns.begin();
		vector<uint64>::const_iterator value = returns.begin();
		for(; symbol != call->returns.end(); ++symbol, ++value)
		{
			context->values[*symbol] = *value;
		}
		
		// Return our value
		return context->values[this];
	}
	if(closure)
	{
		// wcerr << L"≔" << closure->function->label << endl;
		
		// This symbol is defined as a the result of a closure
		Closure* c = closure->close(context);
		uint64 value = reinterpret_cast<uint64>(c);
		context->values[this] = value;
		return value;
	}
	try
	{
		return parse<uint64>(label);
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
