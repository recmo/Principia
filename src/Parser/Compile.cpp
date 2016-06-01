#include "Compile.h"
#include <Parser/Parser.h>
namespace Compile {

template<class T>
std::vector<T> set_to_vector(const std::set<T>& set)
{
	std::vector<T> vec;
	vec.reserve(set.size());
	std::copy(set.begin(), set.end(), std::back_inserter(vec));
	std::sort(vec.begin(), vec.end());
	return vec;
}

std::vector<std::set<uint>> calculate_dependencies(const Parser::Program& p)
{
	std::vector<std::set<uint>> hierarchy(p.calls.size());
	
	// Initialize
	for(uint i = 0; i < p.calls.size(); ++i) {
		auto call = p.calls[i];
		for(uint j = 0; j <  call.size(); ++j) {
			auto ref = call[j];
			if(ref.first != 0 && ref.first != 1 && ref.second != 0)
				hierarchy[i].insert(ref.first);
		}
	}
	
	return hierarchy;
}

std::vector<uint> calculate_closure_calls(const Parser::Program& p)
{
	auto hierarchy = calculate_dependencies(p);
	std::wcerr << hierarchy << "\n";
	
	// Itteratively remove items until only singleton sets remain.
	bool done = false;
	bool failed = false;
	while(!done && !failed) {
		done = true;
		failed = true;
		for(uint i = 0; i < p.calls.size(); ++i) {
			if(hierarchy[i].size() == 1) {
				uint item = *(hierarchy[i].begin());
				for(uint j = 0; j < p.calls.size(); ++j) {
					if(i == j)
						continue;
					if(hierarchy[j].erase(item))
						failed = false;
				}
			}
			if(hierarchy[i].size() > 1)
				done = false;
		}
	}
	
	if(done)
		failed = false;
	std::wcerr << hierarchy << "\n";
	assert(!failed);
	
	// The singletons should create a one-to-one correspondence between
	// closures and calls. (Every call is a tail call, hence every closure
	// has exactly one call).
	
	const uint none = std::numeric_limits<uint>().max();
	std::vector<uint> closure_calls(p.closures.size(), none);
	for(uint call = 0; call < p.calls.size(); ++call) {
		if(hierarchy[call].size() == 1) {
			uint closure = *(hierarchy[call].begin());
			assert(closure_calls[closure - 2] == none);
			closure_calls[closure - 2] = call;
		}
	}
	//std::wcerr << closure_calls << "\n";
	
	// Check if all are matched
	for(auto i: closure_calls)
		assert(i != none);
	
	return closure_calls;
}

std::vector<std::vector<Symbol>> calculate_closures(
	const Parser::Program& p)
{
	const auto hierarchy = calculate_dependencies(p);
	const auto closure_calls = calculate_closure_calls(p);
	
	std::vector<std::set<Symbol>> closures(p.closures.size());
	for(uint closure = 0; closure < p.closures.size(); ++closure) {
		std::vector<Symbol> call = p.calls[closure_calls[closure]];
		for(auto bind: call) {
			if(bind.first == 0 || bind.first == 1)
				continue;
			if(bind.first == closure + 2)
				continue;
			if(bind.second == 0)
				continue;
			closures[closure].insert(bind);
		}
	}
	// std::wcerr << closures << "\n";
	
	// Itteratively add closures
	bool done = false;
	while(!done) {
		done = true;
		for(uint closure = 0; closure < p.closures.size(); ++closure) {
			std::vector<Symbol> call = p.calls[closure_calls[closure]];
			for(auto bind: call) {
				if(bind.first == 0 || bind.first == 1)
					continue;
				if(bind.first == closure + 2)
					continue;
				if(bind.second != 0)
					continue;
				for(auto dep: closures[bind.first - 2]) {
					if(dep.first == closure + 2)
						continue;
					if(closures[closure].insert(dep).second)
						done = false;
				}
			}
		}
	}
	// std::wcerr << closures << "\n";
	
	std::vector<std::vector<Symbol>> sorted;
	for(auto set: closures)
		sorted.push_back(set_to_vector(set));
	// std::wcerr << sorted << "\n";
	
	return sorted;
}

std::vector<std::vector<uint>> calculate_allocs(const Parser::Program& p)
{
	const auto closure_calls = calculate_closure_calls(p);
	const auto closures = calculate_closures(p);
	
	std::vector<std::vector<uint>> allocs;
	for(uint closure = 0; closure < p.closures.size(); ++closure) {
		const auto call = p.calls[closure_calls[closure]];
		std::set<uint> alloc;
		for(auto arg: call) {
			if(arg.first == 0 || arg.first == 1 || arg.first == closure + 2)
				continue;
			if(arg.second != 0)
				continue;
			//std::wcerr << arg << " <- " << closures[arg.first - 2] << "\n";
			alloc.insert(arg.first);
		}
		allocs.push_back(set_to_vector(alloc));
	}
	return allocs;
}

Program compile(const Parser::Program& p)
{
	const auto closure_calls = calculate_closure_calls(p);
	const auto closures = calculate_closures(p);
	const auto allocs = calculate_allocs(p);
	Program program;
	for(uint closure = 0; closure < p.closures.size(); ++closure) {
		Function function;
		function.id = closure + 2;
		function.arity = p.closures[closure];
		if(closure < p.symbols_export.size())
			function.name = p.symbols_export[closure];
		function.closure = closures[closure];
		function.allocations = allocs[closure];
		function.call = p.calls[closure_calls[closure]];
		for(uint i = 0; i < p.closures[closure]; ++i)
			function.arguments.push_back(std::make_pair(closure + 2, i + 1));
		for(auto arg: function.call) {
			if(arg.first == 0)
				function.imports[arg] = p.symbols_import[arg.second];
			else if(arg.first == 1)
				function.constants[arg] = p.constants[arg.second];
		}
		program.push_back(function);
	}
	return program;
}

void write(std::wostream& out, const Compile::Program& p)
{
	for(const Function& f: p) {
		out << L"λ           " << "(" << f.id << L", 0) " << f.name << "\n";
		out << L"Imports     " << f.imports << "\n";
		out << L"Constants   " << f.constants << "\n";
		out << L"Closure     " << f.closure << "\n";
		out << L"Arguments   " << f.arguments << "\n";
		out << L"Allocations " << f.allocations << "\n";
		out << L"Call        " << f.call << "\n";
		out << L"\n";
	}
}

} // namespace Compile
