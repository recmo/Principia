#include "Machine.h"
#include <limits>
namespace Machine {

// There are four kinds of memory addresses
enum address_type {
	type_constant = 0,
	type_closure  = 1,
	type_argument = 2,
	type_alloc    = 3
};

struct address_t {
	unsigned int type  : 2;
	unsigned int index : 14;
	
	bool operator==(const address_t& other) const {
		return type == other.type && index == other.index;
	}
	bool operator!=(const address_t& other) const {
		return !operator==(other);
	}
	bool operator<(const address_t& other) const {
		return (type != other.type) ?
			type < other.type : index < other.index;
	}
};

const uint16_t reference_max = std::numeric_limits<uint16_t>().max();

struct value_t {
	
	uint16_t reference_count = 1;
	
	// Import
	std::wstring import;
	
	// Constant
	std::wstring constant;
	
	// Closure
	uint16_t function_index = 0;
	std::vector<std::shared_ptr<value_t>> values;
	
	bool operator==(const value_t& other) const {
		return import == other.import && constant == other.constant &&
			function_index == other.function_index && values == other.values;
	}
	bool operator!=(const value_t& other) const {
		return !operator==(other);
	}
};

struct alloc_instruction_t {
	uint16_t function_index;
	std::vector<address_t> closure;
};

struct deref_instruction_t {
	address_t address;
};

struct ref_instruction_t {
	address_t address;
	uint16_t  count;
};

struct function_t {
	std::wstring name;
	uint call_count;
	uint16_t closures;
	uint16_t arguments;
	std::vector<deref_instruction_t> derefs;
	std::vector<ref_instruction_t>   refs;
	std::vector<alloc_instruction_t> allocs;
	std::vector<address_t> call;
};

// Program definition

std::vector<std::shared_ptr<value_t>> constants;
std::vector<function_t> functions;

uint main_index;

// Runtime state

bool running = false;
std::shared_ptr<value_t> closure;
std::vector<std::shared_ptr<value_t>> arguments;

void deref(value_t* value)
{
	if(value->reference_count == reference_max) {
		
	}
	value->reference_count -= 1;
	if(value->reference_count == 0) {
		// TODO: Recurse on closure
		delete value;
	}
}

void ref(value_t* value, uint16_t additional)
{
	if(value->reference_count == reference_max)
		return;
	assert(value->reference_count < reference_max - additional);
	value->reference_count += additional;
}

// Optimization passes
void replace_index(function_t& function, address_t from, address_t to);
void remove_unused_allocs(function_t& function);
void update_reference_counts(function_t& function);
void remove_alloc(function_t& function, address_t alloc_index);
bool promote_allocs(function_t& function);
void inline_function(function_t& outer, const function_t& inner);
void inline_functions(std::vector<function_t>& functions);

void print(const function_t& f, uint index);

} std::wostream& operator<<(std::wostream& out, const Machine::address_t& value) {
	switch(value.type) {
	case Machine::type_constant: out << "C"; break;
	case Machine::type_closure:  out << "c"; break;
	case Machine::type_alloc:    out << "A"; break;
	case Machine::type_argument: out << "a"; break;
	}
	out << value.index;
	return out;
} namespace Machine {

} std::wostream& operator<<(std::wostream& out, const Machine::value_t& value) {
	if(!value.import.empty()) {
		out << value.import;
	} else if(!value.constant.empty()) {
		out << L"“" << value.constant << L"”";
	} else {
		out << (unsigned long)value.function_index << "(" << (unsigned long)value.values.size() << ")";
	}
	return out;
} namespace Machine {


address_t make_constant(const value_t& value)
{
	// Find and return existing constant
	for(uint i = 0; i < constants.size(); ++i) {
		if(*constants[i] == value) {
			return address_t{type_constant, i};
		}
	}
	
	// Make a new constant
	const address_t addr{type_constant, constants.size()};
	constants.push_back(std::make_shared<value_t>(value));
	assert(*constants[addr.index] == value);
	constants[addr.index]->reference_count = reference_max;
	return addr;
}

void load(const Compile::Program& program)
{
	// Reset
	constants.clear();
	functions.clear();
	main_index = -1;
	
	for(const Compile::Function& compile_func: program) {
		
		// Find main function
		if(compile_func.name == L"main") {
			main_index = functions.size();
		}
		
		// Map from symbols to memory addresses
		std::map<Parser::Symbol, address_t> symbol_map;
		
		// Layout imports as constants
		for(const auto& import: compile_func.imports) {
			value_t value;
			value.import = import.second;
			symbol_map[import.first] = make_constant(value);
		}
		
		// Layout constant
		for(const auto& constant: compile_func.constants) {
			value_t value;
			value.constant = constant.second;
			symbol_map[constant.first] = make_constant(value);
		}
		
		// Layout closure arguments
		for(uint i = 0; i < compile_func.closure.size(); ++i) {
			symbol_map[compile_func.closure[i]] = address_t{type_closure, i};
		}
		
		// Layout call arguments
		for(uint i = 0; i < compile_func.arguments.size(); ++i) {
			symbol_map[compile_func.arguments[i]] = address_t{type_argument, i};
		}
		
		// Layout allocations
		for(uint i = 0; i < compile_func.allocations.size(); ++i) {
			const Parser::Symbol symbol = std::make_pair(compile_func.allocations[i], 0);
			symbol_map[symbol] = address_t{type_alloc, i};
		}
		
		// Create the machine function
		function_t machine_func;
		machine_func.name = compile_func.name;
		machine_func.call_count = 0;
		machine_func.closures = compile_func.closure.size();
		machine_func.arguments = compile_func.arguments.size();
		
		// Add allocation instructions
		for(uint fid: compile_func.allocations) {
			assert(fid - 2 < program.size());
			if(fid < 2) {
				std::wcerr << "Allocating " << fid << "\n";
				throw "Error";
			}
			const Compile::Function& target = program[fid - 2];
			alloc_instruction_t alloc;
			alloc.function_index = fid - 2;
			for(const Compile::Symbol& symbol: target.closure) {
				alloc.closure.push_back(symbol_map[symbol]);
			}
			machine_func.allocs.push_back(std::move(alloc));
		}
		
		// Add the tail call instruction
		for(const Compile::Symbol& symbol: compile_func.call) {
			if(symbol_map.find(symbol) == symbol_map.end()) {
				std::wcerr << L"Can not find symbol " << symbol << "\n";
				std::wcerr << L"Defined in " << program[symbol.first].name << "\n";
				
				if(compile_func.constants.find(symbol) != compile_func.constants.end()) {
					std::wcerr << L"Constant value " << compile_func.constants.at(symbol) << "\n";
				}
				if(compile_func.imports.find(symbol) != compile_func.imports.end()) {
					std::wcerr << L"Import " << compile_func.imports.at(symbol) << "\n";
				}
			}
			assert(symbol_map.find(symbol) != symbol_map.end());
			machine_func.call.push_back(symbol_map[symbol]);
		}
		
		// Optimize
		promote_allocs(machine_func);
		
		functions.push_back(std::move(machine_func));
	}
	
	// Optimize
	inline_functions(functions);
	// TODO this may create new constant closures, which in turn may lead
	// to new inlining opportunities.
	
	// Update reference counts
	for(function_t& func: functions) {
		remove_unused_allocs(func);
	}
	
	// Make sure we have a valid main
	assert(main_index != -1);
	const function_t& main = functions[main_index];
	assert(main.closures == 0);
	assert(main.arguments == 1);
}

void remove_unused_allocs(function_t& function)
{
	// Find unused allocs
	std::vector<bool> used(function.allocs.size(), false);
	for(const alloc_instruction_t& alloc: function.allocs)
		for(const address_t addr: alloc.closure)
			if(addr.type == type_alloc)
				used[addr.index] = true;
	for(const address_t addr: function.call)
		if(addr.type == type_alloc)
			used[addr.index] = true;
	
	// Remove unused allocs
	std::vector<alloc_instruction_t> new_allocs;
	for(uint i = 0; i < function.allocs.size(); ++i) {
		if(used[i]) {
			new_allocs.push_back(function.allocs[i]);
		}
	}
	function.allocs = new_allocs;
	
	// Relabel addresses
	address_t old_address{type_alloc, 0};
	address_t new_address{type_alloc, 0};
	for(bool is_used: used) {
		if(is_used) {
			assert(old_address.index < used.size());
			assert(new_address.index < function.allocs.size());
			replace_index(function, old_address, new_address);
			old_address.index++;
			new_address.index++;
		} else {
			old_address.index++;
		}
	}
}

void update_reference_counts(function_t& function)
{
	// Remove unused allocs first
	remove_unused_allocs(function);
	
	// Count references of closures, arguments and allocs
	std::map<address_t, uint> reference_count;
	for(uint i = 0; i < function.closures; ++i)
		reference_count[address_t{type_closure, i}] = 0;
	for(uint i = 0; i < function.arguments; ++i)
		reference_count[address_t{type_argument, i}] = 0;
	for(uint i = 0; i < function.allocs.size(); ++i)
		reference_count[address_t{type_alloc, i}] = 0;
	for(const alloc_instruction_t& alloc: function.allocs)
		for(const address_t addr: alloc.closure)
			if(addr.type != type_constant)
				reference_count[addr]++;
	for(const address_t addr: function.call)
		if(addr.type != type_constant)
			reference_count[addr]++;
	if(function.call[0].type != type_constant)
	reference_count[function.call[0]]--;
	
	// Clear existing
	function.derefs.clear();
	function.refs.clear();
	
	// Create new deref and ref instructions
	for(const auto& pair: reference_count) {
		
		// Deref instructions
		if(pair.second == 0) {
			assert(pair.first.type != type_alloc); // Clean up uneccesary allocs first
			function.derefs.push_back(deref_instruction_t{pair.first});
			
		// We inherited a reference on everything, so one means
		// no change
		} else if(pair.second == 1) {
			continue;
			
		// Reference instruction
		} else {
			function.refs.push_back(ref_instruction_t{pair.first, pair.second - 1});
		}
	}
}

void replace_index(function_t& function, address_t from, address_t to)
{
	if(from == to)
		return;
	for(alloc_instruction_t& alloc: function.allocs)
		for(address_t& index: alloc.closure)
			if(index == from)
				index = to;
	for(address_t& index: function.call)
		if(index == from)
			index = to;
}

// Removes an alloc instruction and renumbers indices
void remove_alloc(function_t& function, address_t alloc_address)
{
	assert(alloc_address.type == type_alloc);
	for(alloc_instruction_t& alloc: function.allocs) {
		for(address_t& addr: alloc.closure) {
			assert(addr != alloc_address);
			if(addr.type == type_alloc && (addr.index > alloc_address.index)) {
				addr.index -= 1;
			}
		}
	}
	for(address_t& addr: function.call) {
		assert(addr != alloc_address);
		if(addr.type == type_alloc && (addr.index > alloc_address.index)) {
			addr.index -= 1;
		}
	}
	std::vector<alloc_instruction_t> new_allocs;
	for(uint i = 0; i < function.allocs.size(); ++i) {
		if(i == alloc_address.index)
			continue;
		new_allocs.push_back(function.allocs[i]);
	}
	function.allocs = new_allocs;
}

void inline_function(function_t& outer, const function_t& inner)
{
	// std::wcerr << "Inlining " << inner.name << " in " << outer.name << "\n";
	
	// Inline functions when outer calls inner with a constant closure
	assert(&outer != &inner);
	assert(outer.call.size() == inner.arguments + 1);
	assert(outer.call[0].type == type_constant);
	assert(outer.call[0].index < constants.size());
	const std::shared_ptr<value_t> closure = constants[outer.call[0].index];
	assert(closure->constant.empty());
	assert(closure->import.empty());
	
	// Add closure values to constants
	std::vector<address_t> closure_constants;
	for(const std::shared_ptr<value_t>& value: closure->values) {
		closure_constants.push_back(make_constant(*value));
	}
	
	// Append inner allocs to outer allocs
	const uint alloc_offset = outer.allocs.size();
	std::copy(inner.allocs.begin(), inner.allocs.end(),
		std::back_inserter(outer.allocs));
	
	// Map from inner addresses to outer addresses
	const std::vector<address_t> outer_call = outer.call;
	std::function<address_t(address_t)> map = [&](address_t address) {
		switch(address.type) {
		case type_constant: return address;
		case type_closure:  return closure_constants[address.index];
		case type_alloc:    return address_t{type_alloc, address.index + alloc_offset};
		case type_argument: return outer_call[address.index + 1];
		}
		assert(false);
		return address_t{};
	};
	
	// Remap inner alloc instructions
	for(uint i = alloc_offset; i < outer.allocs.size(); ++i) {
		alloc_instruction_t& alloc = outer.allocs[i];
		for(uint j = 0; j < alloc.closure.size(); ++j) {
			address_t& address = alloc.closure[j];
			address = map(address);
		}
	}
	
	// Replace outer call with remapped inner call
	outer.call = inner.call;
	for(address_t& address: outer.call) {
		address = map(address);
	}
}

// Find constant and/or closure allocs
bool promote_allocs(function_t& function)
{
	bool promoted = false;
	std::vector<bool> alloc_uses_closure;
	std::vector<bool> alloc_uses_arguments;
	
	for(uint i = 0; i < function.allocs.size(); ++i) {
		const address_t alloc_address{type_alloc, i};
		const alloc_instruction_t& alloc = function.allocs[i];
		
		bool uses_closure = false;
		bool uses_arguments = false;
		for(const address_t& addr: alloc.closure) {
			switch(addr.type) {
			case type_constant:
				continue;
			case type_closure:
				uses_closure = true;
				continue;
			case type_argument:
				uses_arguments = true;
				continue;
			case type_alloc:
				uses_closure |= alloc_uses_closure[addr.index];
				uses_arguments |= alloc_uses_arguments[addr.index];
				continue;
			}
		}
		
		// Constant closures
		if(!uses_closure && !uses_arguments) {
			
			// Create a new constant
			value_t value;
			value.function_index = alloc.function_index;
			for(const address_t& addr: alloc.closure) {
				assert(addr.type == type_constant);
				value.values.push_back(constants[addr.index]);
			}
			const address_t addr = make_constant(value);
			
			// Replace alloc by constant
			replace_index(function, alloc_address, addr);
			
			// Remove alloc
			remove_alloc(function, alloc_address);
			
			// We lost our index position
			i -= 1;
			promoted = true;
			continue;
			
		} else if(!uses_arguments) {
			// std::wcerr << "Alloc can be made closure!\n";
		}
		
		alloc_uses_closure.push_back(uses_closure);
		alloc_uses_arguments.push_back(uses_arguments);
	}
	
	return promoted;
}

void inline_functions(std::vector<function_t>& functions)
{
	for(function_t& outer: functions) {
		bool finished = false;
		uint itter = 0;
		while(!finished) {
			finished = true;
			
			// Check the call
			if(outer.call[0].type == type_constant) {
				const std::shared_ptr<value_t>& closure = constants[outer.call[0].index];
				assert(closure->constant.empty());
				
				// We can not inline builtins/imports
				if(!closure->import.empty()) {
					break;
				}
				
				// Inline inner function
				const function_t& inner = functions[closure->function_index];
				inline_function(outer, inner);
				
				// Re-optimize constant closures
				const bool repeat = promote_allocs(outer);
				
				// Repeat
				finished = false;
				++itter;
				
			} else if(outer.call[0].type == type_closure) {
				std::wcerr << "Function " << outer.name << " can have inner closure-inlined\n";
			}
		}
		//std::wcerr << "Inlined " << itter << " times!\n";
	}
}

void print(const function_t& f, uint index)
{
	std::wcerr << "Function " << index << " " << f.name << "\n";
	std::wcerr << "\tcall count: " << f.call_count << "\n";
	std::wcerr << "\tclosures:   " << f.closures << "\n";
	std::wcerr << "\targuments:  " << f.arguments << "\n";
	std::wcerr << "\tallocates:  " << f.allocs.size() << "\n";
	for(const auto& alloc: f.allocs) {
		std::wcerr << "\t\t" << alloc.function_index << " " << alloc.closure << "\n";
	}
	std::wcerr << "\ttail call:\n";
	std::wcerr << "\t\t" << f.call << "\n";
	std::wcerr << "\n";
}

void print()
{
	std::wcerr << "Constants: " << constants << "\n";
	std::wcerr << "Constants: " << constants.size() << "\n";
	return;
	uint index = 0;
	for(const auto& f: functions) {
		print(f, index);
		++index;
	}
}

void run()
{
	// Start at "main" with the built-in "exit" as the only argument
	closure = std::make_shared<value_t>();
	closure->function_index = main_index;
	std::shared_ptr<value_t> exit = std::make_shared<value_t>();
	exit->import = L"exit";
	arguments.clear();
	arguments.push_back(exit);
	running = true;
	while(running) {
		
		// Validate state
		assert(closure != nullptr);
		for(const auto& arg: arguments) {
			assert(arg != nullptr);
		}
		
		// Regular functions
		if(closure->import.empty()) {
			
			// Get the function
			assert(closure->function_index < functions.size());
			function_t& func = functions[closure->function_index];
			assert(closure->values.size() == func.closures);
			assert(arguments.size() == func.arguments);
			func.call_count += 1;
			
			// Create space for allocs
			std::vector<std::shared_ptr<value_t>> allocs;
			allocs.reserve(func.allocs.size());
			
			// Virtual Memory
			std::function<std::shared_ptr<value_t>(address_t)> memory =
				[&](address_t addr) {
				switch(addr.type) {
				case type_constant: return constants[addr.index];
				case type_closure:  return closure->values[addr.index];
				case type_argument: return arguments[addr.index];
				case type_alloc:    return allocs[addr.index];
				}
				assert(false);
				return std::shared_ptr<value_t>();
			};
			
			// Execute alloc instructions
			for(const alloc_instruction_t& inst: func.allocs) {
				std::shared_ptr<value_t> closure = std::make_shared<value_t>();
				closure->function_index = inst.function_index;
				for(const address_t& addr: inst.closure) {
					closure->values.push_back(memory(addr));
				}
				allocs.push_back(std::move(closure));
			}
			
			// Load call instruction
			assert(func.call.size() >= 1);
			std::vector<std::shared_ptr<value_t>> new_arguments;
			new_arguments.reserve(func.call.size() - 1);
			for(uint i = 1; i < func.call.size(); ++i) {
				new_arguments.push_back(memory(func.call[i]));
			}
			closure = memory(func.call[0]);
			arguments = std::move(new_arguments);
			continue;
			
		// Builtin functions
		} else {
			
			if(closure->import == L"print") {
				assert(arguments.size() == 2);
				
				// Print arg₀
				assert(arguments[0]->import.empty());
				assert(arguments[0]->function_index == 0);
				std::wcout << arguments[0]->constant;
				
				// Call arg₁
				closure = arguments[1];
				arguments.clear();
				continue;
			}
			
			if(closure->import == L"exit") {
				
				// Exit
				running = false;
				closure.reset();
				arguments.clear();
				continue;
			}
			
			assert(false);
		}
	}
}

}
