#include "Machine.h"
#include <limits>
#include <algorithm>
#include <sstream>
namespace Machine {

// Program definition

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

struct alloc_instruction_t {
	uint16_t function_index;
	std::vector<address_t> closure;
	
	bool operator==(const alloc_instruction_t& other) const {
		return function_index == other.function_index && closure == other.closure;
	}
	bool operator!=(const alloc_instruction_t& other) const {
		return !operator==(other);
	}
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
	std::vector<alloc_instruction_t> allocs;
	std::vector<ref_instruction_t>   refs;
	std::vector<address_t> call;
};

std::vector<function_t> functions;
uint main_index;

// Runtime state

enum value_type {
	value_closure = 0,
	value_import  = 1,
	value_string  = 2
};

const uint32_t reference_max = 0x3FFF;

struct value_t {
	unsigned int reference_count : 14;
	unsigned int type            :  2;
	
	static value_t* make_closure(uint size) {
		const size_t msize = sizeof(value_t)
			+ sizeof(uint16_t)
			+ size * sizeof(value_t*);
		value_t* r = reinterpret_cast<value_t*>(malloc(msize));
		r->reference_count = 1;
		r->type = value_closure;
		return r;
	}
	
	static value_t* make_import(const std::wstring& value) {
		const size_t msize = sizeof(value_t) + sizeof(std::wstring);
		value_t* r = reinterpret_cast<value_t*>(malloc(msize));
		r->reference_count = 1;
		r->type = value_import;
		new (&(r->string())) std::wstring(value);
		return r;
	}
	
	static value_t* make_string(const std::wstring& value) {
		const size_t msize = sizeof(value_t) + sizeof(std::wstring);
		value_t* r = reinterpret_cast<value_t*>(malloc(msize));
		r->reference_count = 1;
		r->type = value_string;
		new (&(r->string())) std::wstring(value);
		return r;
	}
	
	std::wstring& string() {
		void* data = this + 1;
		return *reinterpret_cast<std::wstring*>(data);
	}
	
	const std::wstring& string() const {
		const void* data = this + 1;
		return *reinterpret_cast<const std::wstring*>(data);
	}
	
	uint16_t& function() {
		void* data = this + 1;
		return *reinterpret_cast<uint16_t*>(data);
	}
	
	const uint16_t& function() const {
		const void* data = this + 1;
		return *reinterpret_cast<const uint16_t*>(data);
	}
	
	value_t** values() {
		void* data = this + 1;
		return reinterpret_cast<value_t**>(data + sizeof(uint16_t));
	}
	
	value_t** values() const {
		void* data = const_cast<value_t*>(this) + 1;
		return reinterpret_cast<value_t**>(data + sizeof(uint16_t));
	}
	
	bool operator==(const value_t& other) const {
		if(this == &other)
			return true;
		if(type != other.type)
			return false;
		switch(type) {
		case value_closure: {
			if(function() != other.function())
				return false;
			const uint size = functions[function()].closures;
			for(uint i = 0; i < size; ++i) {
				if(values()[i] != other.values()[i]) {
					return false;
				}
			}
			return true;
		}
		case value_import:
		case value_string:
			return string() == other.string();
		default:
			return false;
		}
	}
	
	bool operator!=(const value_t& other) const {
		return !operator==(other);
	}
	
private:
	value_t();
} __attribute__((packed));

std::vector<value_t*> constants;

bool running = false;
value_t* closure = nullptr;
std::vector<value_t*> arguments;

// Reference counting
void ref(value_t* value, uint16_t additional);
void deref(value_t* value);
void deref_unpack(value_t* value);

void ref(value_t* value, uint16_t additional)
{
	assert(value != nullptr);
	if(value->reference_count == reference_max)
		return;
	assert(value->reference_count > 0);
	assert(value->reference_count < reference_max - additional);
	value->reference_count += additional;
}

void deref(value_t* value)
{
	assert(value != nullptr);
	if(value->reference_count == reference_max) {
		return;
	}
	assert(value->reference_count > 0);
	value->reference_count -= 1;
	if(value->reference_count == 0) {
		if(value->type == value_closure) {
			const uint size = functions[value->function()].closures;
			for(uint i = 0; i < size; ++i) {
				deref(value->values()[i]);
			}
		}
		free(value);
	}
}

// Combined add reference to children, plus dereference parent
void deref_unpack(value_t* value)
{
	assert(value != nullptr);
	if(value->reference_count == reference_max) {
		return;
	}
	assert(value->type == value_closure);
	assert(value->reference_count > 0);
	value->reference_count -= 1;
	if(value->reference_count == 0) {
		// Destroy the closure, but keep the references to the children.
		free(value);
	} else {
		// Closure was not destroyed, issue new references to the children.
		const uint size = functions[value->function()].closures;
		for(uint i = 0; i < size; ++i) {
			ref(value->values()[i], 1);
		}
	}
}

// Optimization passes
void replace_index(function_t& function, address_t from, address_t to);
void remove_unused_allocs(function_t& function);
void update_reference_counts(function_t& function);
void remove_alloc(function_t& function, address_t alloc_index);
bool promote_allocs(function_t& function);
void inline_function(function_t& outer, const function_t& inner);
void inline_functions(std::vector<function_t>& functions);
void remove_unused_functions_and_constants();
void remove_unused_closures(function_t& function);
void deduplicate_allocs(function_t& function);

void print(const function_t& f, uint index);

	
} std::wostream& operator<<(std::wostream& out, const Machine::alloc_instruction_t& value) {
	out << value.function_index << value.closure;
	return out;
} namespace Machine {

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
	if(value.type == Machine::value_import) {
		out << value.string();
	} else if(value.type == Machine::value_string) {
		out << L"“" << value.string() << L"”";
	} else {
		out << (unsigned long)value.function();
	}
	return out;
} namespace Machine {

} std::wostream& operator<<(std::wostream& out, const Machine::deref_instruction_t& value) {
	out << value.address;
	return out;
} namespace Machine {

} std::wostream& operator<<(std::wostream& out, const Machine::ref_instruction_t& value) {
	out << value.address << ": " << value.count;
	return out;
} namespace Machine {

address_t make_constant(value_t* value)
{
	// Validate
	if(value->type == value_closure) {
		assert(value->function() < functions.size());
	}
	
	// Find and return existing constant
	for(uint i = 0; i < constants.size(); ++i) {
		if(*constants[i] == *value) {
			if(constants[i] != value) {
				free(value);
			}
			return address_t{type_constant, i};
		}
	}
	
	// Make a new constant
	const address_t addr{type_constant, constants.size()};
	constants.push_back(value);
	assert(*constants[addr.index] == *value);
	constants[addr.index]->reference_count = reference_max;
	return addr;
}

void unload()
{
	for(value_t* c: constants) {
		delete c;
	}
	constants.clear();
	functions.clear();
}

void load(const Compile::Program& program)
{
	// Reset
	constants.clear();
	functions.clear();
	main_index = -1;
	
	// Pre-pass to create data structures
	for(const Compile::Function& compile_func: program) {
		function_t machine_func;
		machine_func.name = compile_func.name;
		machine_func.call_count = 0;
		machine_func.closures = compile_func.closure.size();
		machine_func.arguments = compile_func.arguments.size();
		machine_func.allocs.reserve(compile_func.allocations.size());
		functions.push_back(machine_func);
	}
	
	uint index = 0;
	for(const Compile::Function& compile_func: program) {
				
		// Create the machine function
		assert(index < functions.size());
		function_t& machine_func = functions[index];

		// Find main function
		if(compile_func.name == L"main") {
			main_index = index;
		}
		
		// Map from symbols to memory addresses
		std::map<Parser::Symbol, address_t> symbol_map;
		
		// Layout imports as constants
		for(const auto& import: compile_func.imports) {
			value_t* value = value_t::make_import(import.second);
			symbol_map[import.first] = make_constant(value);
		}
		
		// Layout constant
		for(const auto& constant: compile_func.constants) {
			value_t* value = value_t::make_string(constant.second);
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
		
		index++;
	}
	
	// Cleanup
	remove_unused_functions_and_constants();
	
	// Optimize
	for(function_t& func: functions) {
		promote_allocs(func);
	}
	inline_functions(functions);
	
	// Cleanup
	remove_unused_functions_and_constants();
	
	// Update reference counts
	for(function_t& func: functions) {
		remove_unused_allocs(func);
		remove_unused_closures(func);
		update_reference_counts(func);
		deduplicate_allocs(func);
	}
	
	// Make sure we have a valid main
	assert(main_index != -1);
	const function_t& main = functions[main_index];
	assert(main.closures == 0);
	assert(main.arguments == 1);
}

uint constant_index(const value_t* constant)
{
	for(uint i = 0; i < constants.size(); ++i) {
		if(constants[i] == constant) {
			return i;
		}
	}
	return -1;
}

void remove_unused_allocs(function_t& function)
{
	// Find unused allocs
	std::vector<bool> used(function.allocs.size(), false);
	std::function<void(address_t)> use = [&](address_t addr) {
		if(addr.type != type_alloc)
			return;
		if(used[addr.index])
			return;
		used[addr.index] = true;
		for(address_t rec: function.allocs[addr.index].closure)
			use(rec);
	};
	for(const address_t addr: function.call)
		use(addr);
	
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
	// Count references of closures, arguments and allocs
	std::map<address_t, uint> reference_count;
	
	// Initialize counts at zero
	for(uint i = 0; i < function.closures; ++i)
		reference_count[address_t{type_closure, i}] = 0;
	for(uint i = 0; i < function.arguments; ++i)
		reference_count[address_t{type_argument, i}] = 0;
	for(uint i = 0; i < function.allocs.size(); ++i)
		reference_count[address_t{type_alloc, i}] = 0;
	
	// Count references
	for(const alloc_instruction_t& alloc: function.allocs)
		for(const address_t addr: alloc.closure)
			if(addr.type != type_constant)
				reference_count[addr]++;
	for(const address_t addr: function.call)
		if(addr.type != type_constant)
			reference_count[addr]++;
	
	// Note: The call instruction already derefs it's first argument, but it
	// does so in a non-recursive fashion (the closure values get passed to
	// the called function).
	
	// Clear existing
	function.derefs.clear();
	function.refs.clear();
	
	// Create new deref and ref instructions
	for(const auto& pair: reference_count) {
		
		// Deref instructions
		if(pair.second == 0) {
			
			// Realy only unused arguments should be dereffed.
			assert(pair.first.type != type_constant); // Can't deref constants
			assert(pair.first.type != type_alloc); // Clean up uneccesary allocs first
			assert(pair.first.type != type_closure); // Clean up uneccesary closures first
			function.derefs.push_back(deref_instruction_t{pair.first});
			
		// On call, we inherited everything with a reference off one. In most
		// cases we keep this refcount.
		} else if(pair.second == 1) {
			continue;
			
		// Add reference instructions
		} else {
			assert(pair.first.type != type_constant); // Can't ref constants
			function.refs.push_back(ref_instruction_t{pair.first, pair.second - 1});
		}
	}
}

void remap_function_addresses(function_t& function,
	const std::function<address_t(address_t)>& map)
{
	for(alloc_instruction_t& alloc: function.allocs) {
		for(address_t& address: alloc.closure) {
			address = map(address);
		}
	}
	for(address_t& address: function.call) {
		address = map(address);
	}
}

void replace_index(function_t& function, address_t from, address_t to)
{
	remap_function_addresses(function, [=](address_t address) {
		return (address == from) ? to : address;
	});
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

void remove_unused_functions_and_constants()
{
	// Find used functions
	std::vector<bool> used_functions(functions.size(), false);
	std::vector<bool> used_constants(constants.size(), false);
	std::function<void(address_t)> use_address;
	std::function<void(uint)> use_function = [&](uint index) {
		assert(index < functions.size());
		if(used_functions[index])
			return;
		      used_functions[index] = true;
		for(const alloc_instruction_t& alloc: functions[index].allocs) {
			use_function(alloc.function_index);
			for(address_t addr: alloc.closure)
				use_address(addr);
		}
		for(address_t addr: functions[index].call)
			use_address(addr);
	};
	use_address = [&](address_t addr) {
		if(addr.type != type_constant)
			return;
		if(used_constants[addr.index])
			return;
		used_constants[addr.index] = true;
		const value_t* con = constants[addr.index];
		if(con->type != value_closure)
			return;
		assert(con->function() < functions.size());
		use_function(con->function());
		const function_t& func = functions[con->function()];
		for(uint i = 0; i < func.closures; ++i) {
			// All constants (should) have addresses
			use_address(make_constant(con->values()[i]));
		}
	};
	use_function(main_index);
	
	// Remove unused functions
	std::vector<uint> function_map(functions.size(), -1);
	std::vector<function_t> new_functions;
	for(uint i = 0; i < functions.size(); ++i) {
		if(used_functions[i] == false) {
			// std::wcerr << "Remove function " << i << "\n";
		} else {
			function_map[i] = new_functions.size();
			new_functions.push_back(functions[i]);
		}
	}
	functions = new_functions;
	
	// Remap function indices
	main_index = function_map[main_index];
	for(function_t& func: functions) {
		for(alloc_instruction_t& alloc: func.allocs) {
			alloc.function_index = function_map[alloc.function_index];
		}
	}
	for(value_t* con: constants) {
		if(con->type == value_closure) {
			con->function() = function_map[con->function()];
		}
	}
	
	// Remove unused constants
	std::vector<uint> constant_map(constants.size(), -1);
	std::vector<value_t*> new_constants;
	for(uint i = 0; i < constants.size(); ++i) {
		if(used_constants[i] == false) {
			// std::wcerr << "Remove constant " << i << "\n";
			free(constants[i]);
		} else {
	        constant_map[i] = new_constants.size();
			new_constants.push_back(constants[i]);
		}
	}
	constants = new_constants;
	
	// Remap constant indices
	for(function_t& func: functions) {
		for(alloc_instruction_t& alloc: func.allocs) {
			for(address_t& addr: alloc.closure)
				if(addr.type == type_constant)
					addr.index = constant_map[addr.index];
		}
		for(address_t& addr: func.call)
			if(addr.type == type_constant)
				addr.index = constant_map[addr.index];
	}
}

uint16_t find_function_index(const function_t& function)
{
	for(uint i = 0; i < functions.size(); ++i) {
		const function_t& at = functions[i];
		if(&at == &function) {
			return i;
		}
	}
	throw L"Unknown function reference";
}

void remove_function_closure(const uint16_t function_index, const uint16_t closure_index)
{
	// Remove the closure from other functions:
	// * For each alloc of this function:
	// * Remove the current closure from the alloc value list
	// NOTE: Removing the closure from other functions allocs has cascading
	//       effects, and requires a re-optimization of functions.
	
	// For each alloc of function_index
	for(function_t& function: functions) {
		for(alloc_instruction_t& alloc: function.allocs) {
			if(alloc.function_index != function_index) {
				continue;
			}
			
			// Remove the closure parameter at position closure_index
			alloc.closure.erase(alloc.closure.begin() + closure_index);
		}
	}
	
	// Remove the closure from the current function:
	// * Deduct one from closure count
	// * Deduct one from all closure addresses higher than this one
	
	// Remove the entry from the closure parameters
	function_t& func = functions[function_index];
	func.closures -= 1;
	
	// Relabel all closure addresses
	remap_function_addresses(func, [&](address_t address) {
		if(address.type != type_closure) {
			return address;
		}
		if(address.index < closure_index) {
			return address;
		} else if(address.index == closure_index) {
			throw L"Closure was not unused";
		} else {
			assert(address.index > closure_index);
			return address_t{type_closure, address.index - 1};
		}
	});
}

void remove_unused_closures(function_t& function)
{
	std::vector<bool> used(function.closures, false);
	
	// Check uses
	// NOTE: Assumes unused allocs are already removed
	for(const alloc_instruction_t& alloc: function.allocs)
		for(const address_t addr: alloc.closure)
			if(addr.type == type_closure)
				used[addr.index] = true;
	for(const address_t addr: function.call)
		if(addr.type == type_closure)
			used[addr.index] = true;
	
	for(uint i = 0; i < function.closures; ++i) {
		if(used[i])
			continue;
		
		remove_function_closure(find_function_index(function), i);
	}
}

// Find allocs that can be promoted to constants or closures
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
			value_t* value = value_t::make_closure(alloc.closure.size());
			value_t** values = value->values();
			value->function() = alloc.function_index;
			for(const address_t& addr: alloc.closure) {
				assert(addr.type == type_constant);
				*values = constants[addr.index];
				++values;
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

void deduplicate_allocs(function_t& function)
{
	address_t first{type_alloc, 0};
	address_t second{type_alloc, 0};
	for(first.index = 0; first.index < function.allocs.size(); ++first.index) {
		const alloc_instruction_t& first_alloc = function.allocs[first.index];
		for(second.index = first.index + 1; second.index < function.allocs.size(); ++second.index) {
			const alloc_instruction_t& second_alloc = function.allocs[second.index];
			
			// Filter for identical allocs
			if(first_alloc != second_alloc) {
				continue;
			}
			
			// Replace references to second by the first
			replace_index(function, second, first);
			
			// Remove the now redundant second
			remove_alloc(function, second);
			
			// Move the itterator one step back, since
			// we removed the current item
			second.index -= 1;
		}
	}
}

void deduplicate_functions()
{
	// NOTE: Naive comparisson will not detect equivalence
	//       of (mutually) recursive functions.
	
	// Comparisson must be made modulo
	// * Shuffling order of closure arguments
	// * Shuffling order of allocs
	// * Shuffling order of arguments
	// It is assumed constants are already unique
}

void inline_constant_function(function_t& outer)
{
	// Inline functions when outer calls inner with a constant closure
	assert(outer.call.size() > 0);
	assert(outer.call[0].type == type_constant);
	assert(outer.call[0].index < constants.size());
	const value_t* closure = constants[outer.call[0].index];
	assert(closure->type == value_closure);
	assert(closure->function() < functions.size());
	const function_t& inner = functions[closure->function()];
	
	// Add closure values to constants
	std::vector<address_t> closure_constants;
	for(uint i = 0; i < inner.closures; ++i) {
		// make_constant should always return an existing address here.
		closure_constants.push_back(make_constant(closure->values()[i]));
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

void inline_alloc_function(function_t& outer)
{
	// Inline functions when outer calls an alloc it itself created
	assert(outer.call.size() > 0);
	assert(outer.call[0].type == type_alloc);
	assert(outer.call[0].index < outer.allocs.size());
	const alloc_instruction_t alloc = outer.allocs[outer.call[0].index];
	const function_t& inner = functions[alloc.function_index];
	
	// Append inner allocs to outer allocs
	const uint alloc_offset = outer.allocs.size();
	std::copy(inner.allocs.begin(), inner.allocs.end(),
		std::back_inserter(outer.allocs));
	
	// Map from inner addresses to outer addresses
	const std::vector<address_t> outer_call = outer.call;
	std::function<address_t(address_t)> map = [&](address_t address) {
		switch(address.type) {
		case type_constant: return address;
		case type_closure:  return alloc.closure[address.index];
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

void inline_functions(std::vector<function_t>& functions)
{
	for(function_t& outer: functions) {
		bool finished = false;
		uint itterations = 0;
		while(!finished) {
			finished = true;
			
			// (Re)optimize
			remove_unused_allocs(outer);
			promote_allocs(outer);
			remove_unused_closures(outer);
			deduplicate_allocs(outer);
			
			if(outer.call[0].type == type_constant
				&& constants[outer.call[0].index]->type == value_closure) {
				//std::wcerr << "Function " << outer.name << " can have inner constant-inlined\n";
				inline_constant_function(outer);
				finished = false;
				
			} else if(outer.call[0].type == type_alloc) {
				// std::wcerr << "Function " << outer.name << " can have inner alloc-inlined\n";
				inline_alloc_function(outer);
				finished = false;
				
			} else if(outer.call[0].type == type_closure) {
				//std::wcerr << "Function " << outer.name << " can have inner closure-inlined\n";
			}
			
			// Stop after a number of itterations
			++itterations;
			if(itterations > 1000)
				break;
		}
	}
}

void print(const function_t& f, uint index)
{
	std::wcerr << "Function " << index << " " << f.name << "\n";
	std::wcerr << "\tcall count: " << f.call_count << "\n";
	std::wcerr << "\tclosures:   " << f.closures << "\n";
	std::wcerr << "\targuments:  " << f.arguments << "\n";
	std::wcerr << "\tderefs:     " << f.derefs << "\n";
	std::wcerr << "\tallocates:  " << f.allocs.size() << "\n";
	for(const auto& alloc: f.allocs) {
		std::wcerr << "\t\t" << alloc.function_index << " " << alloc.closure << "\n";
	}
	std::wcerr << "\trefs:       " << f.refs << "\n";
	std::wcerr << "\ttail call:\n";
	std::wcerr << "\t\t" << f.call << "\n";
	std::wcerr << "\n";
}

void print()
{
	std::wcerr << "Constants: " << constants << "\n";
	for(value_t* value: constants) {
		std::wcerr << *value << "\n";
	}
	uint index = 0;
	for(const auto& f: functions) {
		print(f, index);
		++index;
	}
}

void validate_reference_counts()
{
	std::map<const value_t*, uint> counts;
	std::function<void(const value_t*)> count = [&](const value_t* value) {
		assert(value != nullptr);
		const bool seen = counts[value] > 0;
		counts[value]++;
		if(!seen && value->type == value_closure) {
			uint size = functions[value->function()].closures;
			for(uint i = 0; i < size; ++i) {
				count(value->values()[i]);
			}
		}
	};
	count(closure);
	for(const value_t* v: arguments) {
		count(v);
	}
	for(const auto& pair: counts) {
		if(pair.first->reference_count == reference_max)
			continue;
		assert(pair.first->reference_count == pair.second);
	}
}

void run()
{
	// Start at "main" with the built-in "exit" as the only argument
	value_t* main = value_t::make_closure(0);
	main->function() = main_index;
	closure = main;
	arguments.clear();
	arguments.push_back(value_t::make_import(L"vm_exit"));
	
	// Allocate arrays out of the loop
	value_t* new_closure;
	std::vector<value_t*> new_arguments;
	std::vector<value_t*> allocs;
	
	// Virtual Memory
	std::function<value_t*(address_t)> memory = [&](address_t addr) -> value_t* {
		switch(addr.type) {
		case type_constant: return constants[addr.index];
		case type_closure:  return closure->values()[addr.index];
		case type_argument: return arguments[addr.index];
		case type_alloc:    return allocs[addr.index];
		}
		assert(false);
		return nullptr;
	};
	
	running = true;
	while(running) {
		
		// Validate state
		// validate_reference_counts();
		
		// Regular functions
		if(closure->type == value_closure) {
			
			// Get the function
			assert(closure->function() < functions.size());
			function_t& func = functions[closure->function()];
			assert(arguments.size() == func.arguments);
			func.call_count += 1;
			
			// Execute deref instructions. Do them recusively
			// since loss of the closure here, means loss of access
			// to the values.
			for(const deref_instruction_t& inst: func.derefs) {
				value_t* value = memory(inst.address);
				deref(value);
			}
			
			// Execute alloc instructions
			for(const alloc_instruction_t& inst: func.allocs) {
				value_t* closure = value_t::make_closure(inst.closure.size());
				closure->function() = inst.function_index;
				value_t** values = closure->values();
				for(const address_t& addr: inst.closure) {
					*values = memory(addr);
					++values;
				}
				allocs.push_back(closure);
			}
			
			// Execute ref instructions
			for(const ref_instruction_t& inst: func.refs) {
				value_t* value = memory(inst.address);
				ref(value, inst.count);
			}
			
			// Load call instruction
			assert(func.call.size() >= 1);
			new_arguments.clear();
			for(uint i = 1; i < func.call.size(); ++i) {
				value_t* value = memory(func.call[i]);
				new_arguments.push_back(value);
			}
			new_closure = memory(func.call[0]);
			
			// Deref the called closure and add a reference to all the values.
			// We can do this in one operation, to save in the common case
			// where the closure's refcount is one.
			deref_unpack(closure);
			
			// Reset & repeat
			allocs.clear();
			closure = new_closure;
			std::swap(arguments, new_arguments);
			continue;
			
		// Builtin functions
		} else {
			assert(closure->type == value_import);
			
			if(closure->string() == L"print") {
				assert(arguments.size() == 2);
				
				// Print arg₀
				assert(arguments[0]->type == value_string);
				std::wcout << arguments[0]->string();
				deref(arguments[0]);
				
				// Call arg₁
				deref(closure);
				closure = arguments[1];
				arguments.clear();
				continue;
			}
			
			if(closure->string() == L"read") {
				assert(arguments.size() == 1);
				
				// Read from stdin
				std::wstring str;
				std::getline(std::wcin, str);
				value_t* value = value_t::make_string(str);
				
				// Call arg₁ with the new string as argument
				deref(closure);
				closure = arguments[1];
				arguments.clear();
				arguments.push_back(value);
				continue;
			}
			
			if(closure->string() == L"vm_exit") {
				assert(arguments.size() == 0);
				
				// Exit
				deref(closure);
				running = false;
				closure = nullptr;
				arguments.clear();
				continue;
			}
			
			std::wcerr << "Unknown builtin: " << closure->string() << "\n";
			assert(false);
		}
	}
}

std::wstring reg_allocator(uint function_index, address_t address)
{
	// Constants do not need registers
	if(address.type == type_constant) {
		std::wstringstream ss;
		ss << "constant_" << address.index;
		return ss.str();
	}
	
	// X86-64 registers
	const std::wstring regs[] = {
		L"rax", L"rbx", L"rcx", L"rdx",
		L"rbp", // rsp, rsi, rdi, (These are rather special and skipped)
		L"r8",  L"r9",  L"r10", L"r11",
		L"r12", L"r13", L"r14", L"r15"
	};
	
	const function_t& func = functions[function_index];
	uint index = 0;
	if(address.type == type_argument) {
		index += address.index;
		assert(index < sizeof(regs));
		return regs[index];
	} else {
		index += func.arguments;
	}
	if(address.type == type_closure) {
		index += address.index;
		assert(index < sizeof(regs));
		return regs[index];
	} else {
		index += func.closures;
	}
	if(address.type == type_alloc) {
		index += address.index;
		assert(index < sizeof(regs));
		return regs[index];
	}
	throw "Invalid address";
}

uint closure_size(uint num_values)
{
	return 12 + 8 * num_values;
}

void assemble()
{
	// Find the maximum closure size
	uint16_t max_size = 0;
	for(const function_t& func: functions)
		max_size = std::max(max_size, func.closures);
	
	std::wcout <<
		"; Output from Principia\n"
		"; nasm -f elf64 -\n"
		"%assign max_size " << max_size << "\n"
		"%include \"runtime.asm\"\n"
		"\n"
		"section .rodata\n"
		"\n";
	
	for(uint i = 0; i < constants.size(); ++i) {
		const value_t* value = constants[i];
		if(value->type == value_string) {
			std::wstring str = value->string();
			std::wcout <<
				"constant_" << i << ": ; string\n"
				"	dw 0x0000        ; reference_count (zero for static)\n"
				"	dw 0x0000        ; size\n"
				"	dq type_string   ; function (type_string)\n"
				"	dd " << str.size() << "            ; num_bytes\n";
			if(str == L"\n") {
				std::wcout <<
					"	dd 10            ; \n"
					"\n";
			} else {
				std::wcout <<
					"	db \"" << str << "\"\n"
					"\n";
			}
		}
		if(value->type == value_import) {
			uint16_t index = 0;
			if(value->string() == L"exit")
				index = 5;
			if(value->string() == L"print")
				index = 6;
			if(value->string() == L"read")
				index = 7;
			std::wcout <<
				"constant_" << i << ": ; Static closure for " << value->string() << " \n"
				"	dw 0x0000        ; reference_count (zero for static)\n"
				"	dw 0x0000        ; size\n"
				"	dq " << value->string() << " ; function pointer\n"
				"\n";
		}
		if(value->type == value_closure) {
			const uint16_t index = value->function();
			const function_t& func = functions[index];
			std::wcout <<
				"constant_" << i << ": ; Static closure for " << func.name << " \n"
				"	dw 0x0000        ; reference_count (zero for static)\n"
				"	dw " << func.closures << "   ; size\n"
				"	dq func_" << index << " ; function pointer\n";
			for(uint j = 0; j < func.closures; ++j) {
				const uint ref_index = constant_index(value->values()[j]);
				std::wcout <<
					"	dq constant_" << ref_index << "\n";
			}
			std::wcout <<
				"\n";
		}
	}
	
	std::wcout <<
		"section .text\n"
		"\n";
	
	for(uint i = 0; i < functions.size(); ++i) {
		const function_t& func = functions[i];
		
		if(i == main_index) {
			std::wcout << "main: \n";
		}
		
		std::wcout << "func_" << i << ": ; " << func.name << "\n";
		
		// Unpack closures for functions with non-constant closures
		if(func.closures > 0) {
			std::wcout <<
				"	; Unpack closure of size " << func.closures << "\n"
				"	add rsp, 12\n";
			for(uint j = 0; j < func.closures; ++j) {
				std::wcout <<
					"	pop " << reg_allocator(i, address_t{type_closure, j}) << "\n";
			}
			std::wcout <<
				"	sub rsp, " << (12 + 8 * func.closures) << "\n"
				"	mov rdi, .unpacked  ; return address\n"
				"	jmp mem_unpack      ; closure in rsp\n"
				"	.unpacked:\n"
				"	\n";
		}
		
		for(uint j = 0; j < func.derefs.size(); ++j) {
			const deref_instruction_t& deref = func.derefs[j];
			const uint r = 1 + func.allocs.size() + func.refs.size() + j;
			std::wcout <<
				"	; Deref " << deref << "\n"
				"	mov rsi, " << reg_allocator(i, deref.address) << "\n"
				"	mov rdi, .ret_" << r << "\n"
				"	jmp mem_deref\n"
				"	.ret_" << r << ":\n"
				"	\n";
		}
		
		for(uint j = 0; j < func.allocs.size(); ++j) {
			const alloc_instruction_t& alloc = func.allocs[j];
			
			// Compute the total ref_count
			uint16_t ref_count = 1;
			for(uint k = 0; k < func.refs.size(); ++k) {
				const ref_instruction_t& ref = func.refs[k];
				if(ref.address.type == type_alloc && ref.address.index == j) {
					ref_count += ref.count;
				}
			}
			
			// Do not emit an alloc construct if we are only going to call it
			// (in that case, we pass the closure variables in registers)
			if(func.call[0].type == type_alloc && func.call[0].index == j && ref_count == 1) {
				continue;
			}
			
			const std::wstring reg = reg_allocator(i, address_t{type_alloc, j});
			assert(alloc.closure.size() > 0); // Can not alloc constants
			std::wcout <<
				"	; Alloc " << alloc << "\n"
				"	mov rsi, " << (12 + 8 * alloc.closure.size()) << "\n"
				"	mov rdi, .ret_" << (j + 1) << "\n"
				"	jmp mem_alloc\n"
				"	.ret_" << (j + 1) << ":\n"
				"	add rsp, " << (12 + 8 * alloc.closure.size()) << "\n";
			for(uint k = 0; k < alloc.closure.size(); ++k) {
				std::wcout <<
					"	push " <<
					reg_allocator(i, alloc.closure[alloc.closure.size() - k - 1]) << "\n";
			}
			std::wcout <<
				"	push func_"<< alloc.function_index << "\n"
				"	push word " << alloc.closure.size() << "\n"
				"	push word " << ref_count << "\n"
				"	mov " << reg << ", rsp\n"
				"	\n";
		}
		
		for(uint j = 0; j < func.refs.size(); ++j) {
			const ref_instruction_t& ref = func.refs[j];
			if(ref.address.type == type_alloc) {
				continue; // These are handled in alloc above
			}
			const uint r = 1 + func.allocs.size() + j;
			std::wcout <<
				"	; Ref " << ref << "\n"
				"	movzx rsi, word [" << reg_allocator(i, ref.address) << "] ; Load ref_count\n"
				"	test rsi, rsi      ; Test for zero\n"
				"	jz .ret_" << r << "          ; Skip if zero\n"
				"	add rsi, " << ref.count << "         ; Increase reference count\n"
				"	mov word [" << reg_allocator(i, ref.address) << "], si ; Store reference count\n"
				"	.ret_" << r << ":            ;\n"
				"	\n";
		}
		
		std::wcout << "	; Call " << func.call << "\n";
		std::wcout << "	mov rsp, " << reg_allocator(i, func.call[0]) << "\n";
		
		// We need to re-arrange the registers to match with the call arguments
		// TODO: Find permutations and do cycle decomposition into xchg instructions
		std::vector<std::wstring> old_registers;
		std::vector<std::wstring> new_registers;
		for(uint j = 1; j < func.call.size(); ++j) {
			old_registers.push_back(reg_allocator(i, func.call[j]));
			new_registers.push_back(reg_allocator(i, address_t{type_argument, j - 1}));
		}
		//std::wcerr << old_registers << " -> " << new_registers << "\n";
		
		// This naive approach works for the Adams example (since it has no
		// register permutations). We first set the constants, then the
		// non-constants
		for(uint j = 0; j < func.call.size() - 1; ++j) {
			if(func.call[j + 1].type == type_constant) {
				continue;
			}
			const std::wstring reg = reg_allocator(i, address_t{type_argument, j});
			std::wcout <<
				"	mov " << reg << ", " << reg_allocator(i, func.call[j + 1]) << "\n";
		}
		for(uint j = 0; j < func.call.size() - 1; ++j) {
			if(func.call[j + 1].type != type_constant) {
				continue;
			}
			const std::wstring reg = reg_allocator(i, address_t{type_argument, j});
			std::wcout <<
				"	mov " << reg << ", " << reg_allocator(i, func.call[j + 1]) << "\n";
		}
		
		// Jump to the next function
		if(func.call[0].type == type_constant) {
			const value_t* closure = constants[func.call[0].index];
			if(closure->type == value_import) {
				std::wcout <<
					"	jmp " << closure->string() << " ; Jump to next function\n"
					"\n";
			} else {
				std::wcout <<
					"	jmp func_" << closure->function()<< " ; Jump to next function\n"
					"\n";
			}
		} else if (func.call[0].type == type_alloc) {
			const alloc_instruction_t& alloc = func.allocs[func.call[0].index];
			uint index = alloc.function_index;
			for(uint j = 0; j < alloc.closure.size(); ++j) {
				const std::wstring src = reg_allocator(i, alloc.closure[j]);
				const std::wstring dest = reg_allocator(index, address_t{type_closure, j});
				// TODO: These registers should also be taken in the permutation resolver
				std::wcout <<
					"	mov " << dest << ", " << src << " ; Closure in registers \n";
			}
			std::wcout <<
				"	jmp func_" << index << ".unpacked; Jump to next function\n"
				"\n";
			
		} else {
			std::wcout <<
				"	jmp [rsp + 4] ; Jump to next function\n"
				"\n";
		}
	}
}

}
