#include "Interpreter.h"
#include "fixups.h"
#include "Symbol.h"
#include "CallDefinition.h"
#include "ClosureDefinition.h"

using namespace std;

Interpreter::Interpreter()
{
}

void Interpreter::processLine(const wstring& line)
{
	vector<wstring> tokens = tokenize(line);
	if(tokens.empty()) return;
	
	if(line == L"dump")
	{
		for(auto it = program.symbols.begin(); it != program.symbols.end(); ++it)
		{
			wcerr << it->first << L": ";
			wcerr << it->second->label << L"(";
			wcerr << it->second->call << L", " << it->second->closure << L")";
			wcerr << endl;
		}
		for(auto it = context.values.begin(); it != context.values.end(); ++it)
		{
			wcerr << it->first->label << L" = " << it->second << endl;
		}
		return;
	}
	
	// Parse <outputs> "≔" <func> <inputs>
	vector<wstring>::iterator call_sign;
	call_sign = find(tokens.begin(), tokens.end(), L"≔");
	if(call_sign != tokens.end())
	{
		if(call_sign + 1 == tokens.end())
		{
			wcerr << L"Call should have at least a function name" << endl;
			return;
		}
		wstring func;
		vector<wstring> inputs;
		vector<wstring> outputs;
		copy(tokens.begin(), call_sign, back_inserter<vector<wstring> >(outputs));
		func = *(call_sign + 1);
		copy(call_sign + 2, tokens.end(), back_inserter<vector<wstring> >(inputs));
		addCall(func, inputs, outputs);
		return;
	}
	
	// Parse <func> <arguments> ↦ <returns>
	vector<wstring>::iterator closure_sign;
	closure_sign = find(tokens.begin(), tokens.end(), L"↦");
	if(closure_sign != tokens.end())
	{
		if(closure_sign == tokens.begin())
		{
			wcerr << L"Closure should have at least a function name" << endl;
			return;
		}
		wstring func;
		vector<wstring> arguments;
		vector<wstring> returns;
		func = *(tokens.begin());
		copy(tokens.begin() + 1, closure_sign, back_inserter<vector<wstring> >(arguments));
		copy(closure_sign + 1, tokens.end(), back_inserter<vector<wstring> >(returns));
		addClosure(func, arguments, returns);
		return;
	}
	
	// Parse <symbol>
	if(tokens.size() == 1)
	{
		wstring label= *(tokens.begin());
		map<wstring, Symbol*>::iterator it;
		it = program.symbols.find(label);
		if(it == program.symbols.end())
		{
			wcerr << L"Unknown symbol " << label << L"." << endl;
			return;
		}
		Symbol* symbol =  it->second;
		wcerr << L"Evaluating " << symbol->label << L"…" << endl;
		Value value = symbol->evaluate(&context);
		wcout << symbol->label << L" = " << value << "." << endl;
		return;
	}
	
	// Nothing matched!
	wcerr << L"Error: Could not parse line." << endl;
}

void Interpreter::addCall(const std::wstring& func_label, const std::vector<wstring>& input_labels, const std::vector<wstring>& output_labels)
{
	vector<wstring>::const_iterator it;
	wcerr << output_labels << L" ≔ " << func_label << L" " << input_labels << endl;
	CallDefinition* call = new CallDefinition;
	
	// Set the function
	call->function = getOrCreateSymbol(func_label);
	
	// Set the arguments
	for(it = input_labels.begin(); it != input_labels.end(); ++it)
	{
		call->arguments.push_back(getOrCreateSymbol(*it));
	}
	
	// Set the returned values
	for(it = output_labels.begin(); it != output_labels.end(); ++it)
	{
		Symbol* output = getOrCreateSymbol(*it);
		if(output->call != 0 || output->call != 0)
		{
			wcerr << L"Error: Symbol " << *it << L" can only be assigned once." << endl;
			/// TODO: Unroll, delete all the other produced symbols
			return;
		}
		output->call = call;
		call->returns.push_back(output);
	}
}

void Interpreter::addClosure(const std::wstring& func_label, const std::vector<wstring>& argument_labels, const std::vector<wstring>& return_labels)
{
	wcerr << func_label << L" " << argument_labels << L" ↦ " << return_labels << endl;
	vector<wstring>::const_iterator it;
	ClosureDefinition* closure = new ClosureDefinition;
	
	// Set function symbol
	closure->function = getOrCreateSymbol(func_label);
	if(closure->function->call != 0 || closure->function->closure != 0)
	{
		wcerr << L"Error: Symbol " << func_label << L" can only be assigned once." << endl;
		/// TODO: Unroll, delete all the other produced symbols
		return;
	}
	closure->function->closure = closure;
	
	// Set returns
	for(it = return_labels.begin(); it != return_labels.end(); ++it)
	{
		Symbol* ret = getOrCreateSymbol(*it);
		closure->returns.push_back(ret);
	}
	
	// Set arguments
	for(it = argument_labels.begin(); it != argument_labels.end(); ++it)
	{
		Symbol* arg = getOrCreateSymbol(*it);
		if(arg->call != 0 || arg->closure != 0)
		{
			wcerr << L"Error: Symbol " << *it << L" can only be assigned once." << endl;
			/// TODO: Unroll, delete all the other produced symbols
			return;
		}
		arg->closure = closure;
		closure->arguments.push_back(arg);
	}
}

Symbol* Interpreter::getOrCreateSymbol(const std::wstring& label)
{
	Symbol* symbol = program.symbols[label];
	if(symbol == 0)
	{
		symbol = new Symbol;
		symbol->label = label;
		program.symbols[label] = symbol;
	}
	return symbol;
}
