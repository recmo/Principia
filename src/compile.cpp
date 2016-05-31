#include <Command.h>
#include <Parser/Parser.h>
#include <vector>
#include <set>
#include <limits>

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

void calculate_closure_hierarchy(const Parser::Program& p)
{
	auto hierarchy = calculate_dependencies(p);
	
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
	std::vector<uint> call_closures(p.calls.size(), none);
	for(uint call = 0; call < p.calls.size(); ++call) {
		if(hierarchy[call].size() == 1) {
			uint closure = *(hierarchy[call].begin());
			assert(closure_calls[closure - 2] == none);
			assert(call_closures[call] == none);
			closure_calls[closure - 2] = call;
			call_closures[call] = closure - 2;
		}
	}
	std::wcerr << closure_calls << "\n";
	std::wcerr << call_closures << "\n";
	
	// TODO: Compute the closures
	
	std::vector<std::set<std::pair<uint,uint>>> closures(p.closures.size());
	for(uint closure = 0; closure < p.closures.size(); ++closure) {
		std::vector<std::pair<uint,uint>> call = p.calls[closure_calls[closure]];
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
	std::wcerr << closures << "\n";
	
	// Itteratively add closures
	{
		bool done = false;
		while(!done) {
			done = true;
			for(uint closure = 0; closure < p.closures.size(); ++closure) {
				std::vector<std::pair<uint,uint>> call = p.calls[closure_calls[closure]];
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
	}
	std::wcerr << closures << "\n";
	
	// constants: [constants..]
	// closure:   [c1 c2 c3...]
	// arguments: [a1 a2 a3...]
	//
	// λ
	// x1 = [λ1 const1 c2 a2]
	// x2 = [λ2 c1 c3 x1]
	// x3 = [λ3]
	// a3 const1 c1 x1 x2 x3 a4
	
	for(uint closure = 0; closure < p.closures.size(); ++closure) {
		
		std::wcerr << "\n(" << (closure + 2) << L", 0) λ ";
		if(closure < p.symbols_export.size())
			std::wcerr << p.symbols_export[closure];
		std::wcerr << "\n";
		const auto call = p.calls[closure_calls[closure]];
		
		// Builtins
		std::wcerr << L"Builtins: ";
		for(auto arg: call)
			if(arg.first == 0)
				std::wcerr << arg << " = " << p.symbols_import[arg.second] << " ";
		std::wcerr << L"\n";
		
		// Constants
		std::wcerr << L"Constants: ";
		for(auto arg: call)
			if(arg.first == 1)
				std::wcerr << arg << " ";
		std::wcerr << L"\n";
		
		// Closure
		std::wcerr << L"Closure: " << closures[closure] << L"\n";
		
		// Arguments
		std::wcerr << L"Arguments: (" << (closure + 2) << ", 0)";
		for(uint i = 0; i < p.closures[closure]; ++i)
			std::wcerr << L" (" << (closure + 2) << ", " << (i + 1) << L")";
		std::wcerr << L"\n";
		
		// New closures
		for(auto arg: call) {
			if(arg.first == 0 || arg.first == 1 || arg.first == closure + 2)
				continue;
			if(arg.second != 0)
				continue;
			std::wcerr << arg << " <- " << closures[arg.first - 2] << "\n";
		}
		
		// Call
		std::wcerr << call << L"\n";
		
		
	}
}

Command compile(L"compile", [](Command::Arguments arguments) {
	assert(arguments.size() == 1);
	
	Parser::Program p = Parser::compile(Parser::parseFile(arguments[0]));
	
	Parser::write(std::wcerr, p);
	calculate_closure_hierarchy(p);
	
	return Command::success;
},
L"parse a source file",
L"<source file>\n\n"
"The source file is parsed and the resulting structure printed to\n"
"the standard output.\n"
);
