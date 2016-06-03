#include "Interpreter.h"
#include <Unicode/exceptions.h>
#include <Utilities/pretty_print.h>
#include <map>

std::wostream& operator<<(std::wostream& out, const Interpreter::Value& value)
{
	if(value.kind == Interpreter::Import)
		out << value.string;
	if(value.kind == Interpreter::Constant)
		out << L"“" << value.string << L"”";
	if(value.kind == Interpreter::Closure)
		out << L"λ" << value.closure << value.values;
	return out;
}

namespace Interpreter {

void run(const Program& p, const std::wstring& func)
{
	for(const auto& f: p)
		if(f.name == func)
			return run(p, f);
	throw runtime_error(L"Could not find function.");
}

void run(const Program& p, const Function& f)
{
	assert(f.closure.empty());
	Value exit;
	exit.kind = Import;
	exit.string = L"exit";
	run(p, f, Values(), Values(f.arguments.size(), exit));
}

void run(const Program& p, const Function& f, const Values& c, const Values& a)
{
	// std::wcerr << "TRACE " << f.id << " " << f.name << " " << c << " " << a << "\n";
	assert(c.size() == f.closure.size());
	assert(a.size() == f.arguments.size());
	std::map<Symbol, Value> map;
	for(const auto& import: f.imports) {
		Value v;
		v.kind = Import;
		v.string = import.second;
		map[import.first] = v;
	}
	for(const auto& constant: f.constants) {
		Value v;
		v.kind = Constant;
		v.string = constant.second;
		map[constant.first] = v;
	}
	for(uint i = 0; i < c.size(); ++i)
		map[f.closure[i]] = c[i];
	for(uint i = 0; i < a.size(); ++i)
		map[f.arguments[i]] = a[i];
	for(uint alloc: f.allocations) {
		assert(alloc - 2 < p.size());
		const Function& af = p[alloc - 2];
		assert(af.id == alloc);
		
		Value ac;
		ac.kind = Closure;
		ac.closure = alloc;
		for(const Symbol& s: af.closure)
			ac.values.push_back(map[s]);
		map[std::make_pair(alloc, 0)] = ac;
	}
	//std::wcerr << map << "\n";
	Values ca;
	for(uint i = 1; i < f.call.size(); ++i)
		ca.push_back(map[f.call[i]]);
	run(p, map[f.call[0]], ca);
}

void run(const Program& p, const Value& function, const Values& arguments)
{
	if(function.kind == Import)
		return run_builtin(p, function.string, arguments);
	//std::wcerr << "CALL " << function << " " << arguments << "\n";
	
	assert(function.kind == Closure);
	assert(function.closure - 2 < p.size());
	const Function& af = p[function.closure - 2];
	assert(af.id == function.closure);
	run(p, af, function.values, arguments);
}

void run_builtin(const Program& p, const std::wstring& function,
	const Values& arguments)
{
	if(function == L"exit") {
		std::wcerr << L"EXIT: " << arguments << "\n";
		return;
	}
	if(function == L"print") {
		std::wcout << arguments[0].string;
		return run(p, arguments[1], Values());
	}
	std::wcerr << L"Unknown function " << arguments[0].string << "\n";
}

};
