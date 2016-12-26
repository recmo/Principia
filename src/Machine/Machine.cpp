#include "Machine.h"
namespace Machine {

// There are four kinds of memory addresses
const uint16_t type_mask     = 0xC000;
const uint16_t type_constant = 0x0000;
const uint16_t type_closure  = 0x4000;
const uint16_t type_argument = 0x8000;
const uint16_t type_alloc    = 0xC000;

struct value_t {
	
	// Import
	std::wstring import;
	
	// Constant
	std::wstring constant;
	
	// Closure
	uint16_t function_index;
	std::vector<std::shared_ptr<value_t>> values;
};

struct alloc_instruction_t {
	uint16_t function_index;
	std::vector<uint16_t> closure;
};

struct function_t {
	uint call_count;
	std::vector<std::shared_ptr<value_t>> constants;
	uint16_t closures;
	uint16_t arguments;
	std::vector<alloc_instruction_t> allocs;
	std::vector<uint16_t> call;
};

// Program definition

std::vector<function_t> functions;

uint main_index;

// Runtime state

bool running = false;
std::shared_ptr<value_t> closure;
std::vector<std::shared_ptr<value_t>> arguments;

void optimize(function_t& function);

void load(const Compile::Program& program)
{
	// Reset
	functions.clear();
	main_index = -1;
	
	for(const Compile::Function& compile_func: program) {
		
		// Find main function
		if(compile_func.name == L"main") {
			main_index = functions.size();
		}
		
		// Map from symbols to memory addresses
		std::map<Parser::Symbol, uint16_t> symbol_map;
		
		// Create empty stack, list of imports and list of constants
		std::vector<std::shared_ptr<value_t>> constants;
		
		// Layout imports as constants
		for(const auto& import: compile_func.imports) {
			symbol_map[import.first] = type_constant | constants.size();
			std::shared_ptr<value_t> value = std::make_shared<value_t>();
			value->import = import.second;
			constants.push_back(value);
		}
		
		// Layout constant
		for(const auto& constant: compile_func.constants) {
			symbol_map[constant.first] = type_constant | constants.size();
			std::shared_ptr<value_t> value = std::make_shared<value_t>();
			value->constant = constant.second;
			constants.push_back(value);
		}
		
		// Layout closure arguments
		for(uint i = 0; i < compile_func.closure.size(); ++i) {
			symbol_map[compile_func.closure[i]] = type_closure | i;
		}
		
		// Layout call arguments
		for(uint i = 0; i < compile_func.arguments.size(); ++i) {
			symbol_map[compile_func.arguments[i]] = type_argument | i;
		}
		
		// Layout allocations
		for(uint i = 0; i < compile_func.allocations.size(); ++i) {
			const Parser::Symbol symbol = std::make_pair(compile_func.allocations[i], 0);
			symbol_map[symbol] = type_alloc | i;
		}
		
		// Create the machine function
		function_t machine_func;
		machine_func.call_count = 0;
		machine_func.constants = std::move(constants);
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
		optimize(machine_func);
		
		functions.push_back(std::move(machine_func));
	}
	
	// Make sure we have a valid main
	assert(main_index != -1);
	const function_t& main = functions[main_index];
	assert(main.closures == 0);
	assert(main.arguments == 1);
}

void replace_index(function_t& function, uint16_t from, uint16_t to)
{
	for(alloc_instruction_t& alloc: function.allocs) {
		for(uint16_t& index: alloc.closure) {
			if(index == from) {
				index = to;
			}
		}
	}
	for(uint16_t& index: function.call) {
		if(index == from) {
			index = to;
		}
	}
}

// Removes an alloc instruction and renumbers indices
void remove_alloc(function_t& function, uint16_t alloc_index)
{
	assert((alloc_index & type_mask) == type_alloc);
	assert((alloc_index & ~type_mask) < function.allocs.size());
	for(alloc_instruction_t& alloc: function.allocs) {
		for(uint16_t& index: alloc.closure) {
			assert(index != alloc_index);
			if((index & type_mask) == type_alloc && (index > alloc_index)) {
				--index;
			}
		}
	}
	for(uint16_t& index: function.call) {
		assert(index != alloc_index);
		if((index & type_mask) == type_alloc && (index > alloc_index)) {
			--index;
		}
	}
	std::vector<alloc_instruction_t> new_allocs;
	for(uint i = 0; i < function.allocs.size(); ++i) {
		if((i | type_alloc) == alloc_index)
			continue;
		new_allocs.push_back(function.allocs[i]);
	}
	function.allocs = new_allocs;
}

// Find constant and/or closure allocs
void optimize(function_t& function)
{
	std::vector<bool> alloc_uses_closure;
	std::vector<bool> alloc_uses_arguments;
	
	for(uint i = 0; i < function.allocs.size(); ++i) {
		const uint16_t alloc_address = type_alloc | i;
		const alloc_instruction_t& alloc = function.allocs[i];
		
		bool uses_closure = false;
		bool uses_arguments = false;
		for(uint16_t index: alloc.closure) {
			const uint16_t type = index & type_mask;
			index &= ~type_mask;
			switch(type) {
			case type_constant:
				continue;
			case type_closure:
				uses_closure = true;
				continue;
			case type_argument:
				uses_arguments = true;
				continue;
			case type_alloc:
				uses_closure |= alloc_uses_closure[index];
				uses_arguments |= alloc_uses_arguments[index];
				continue;
			}
		}
		
		if(!uses_closure && !uses_arguments) {
			std::shared_ptr<value_t> value = std::make_shared<value_t>();
			value->function_index = alloc.function_index;
			for(uint16_t index: alloc.closure) {
				assert((index & type_mask) == type_constant);
				index &= ~type_mask;
				value->values.push_back(function.constants[index]);
			}
			
			// Create a new constant
			uint16_t index = type_constant | function.constants.size();
			function.constants.push_back(value);
			
			// Replace alloc by constant
			replace_index(function, alloc_address, index);
			
			// Remove alloc
			remove_alloc(function, alloc_address);
			
			// We lost our index position
			index -= 1;
			continue;
			
		} else if(!uses_arguments) {
			// std::wcerr << "Alloc can be made closure!\n";
		}
		
		alloc_uses_closure.push_back(uses_closure);
		alloc_uses_arguments.push_back(uses_arguments);
	}
}

void print(const Compile::Program& program)
{
	uint index = 0;
	for(const auto& f: functions) {
		const Compile::Function& cf = program[index];
		
		std::wcerr << "Function " << index << " " << cf.name << "\n";
		std::wcerr << "\tcall count: " << f.call_count << "\n";
		std::wcerr << "\tconstants:  " << f.constants.size() << "\n";
		std::wcerr << "\tclosures:   " << f.closures << "\n";
		std::wcerr << "\targuments:  " << f.arguments << "\n";
		std::wcerr << "\tallocates:  " << f.allocs.size() << "\n";
		for(const auto& alloc: f.allocs) {
			std::wcerr << "\t\t" << alloc.function_index << " " << alloc.closure << "\n";
		}
		std::wcerr << "\ttail call:\n";
		std::wcerr << "\t\t" << f.call << "\n";
		std::wcerr << "\n";
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
			std::function<std::shared_ptr<value_t>(uint)> memory = [&](uint index) {
				const uint type = index & type_mask;
				index &= ~type_mask;
				switch(type) {
				case type_constant: return func.constants[index];
				case type_closure:  return closure->values[index];
				case type_argument: return arguments[index];
				case type_alloc:    return allocs[index];
				}
				assert(false);
				return std::shared_ptr<value_t>();
			};
			
			// Execute alloc instructions
			for(const alloc_instruction_t& inst: func.allocs) {
				std::shared_ptr<value_t> closure = std::make_shared<value_t>();
				closure->function_index = inst.function_index;
				for(uint16_t index: inst.closure) {
					closure->values.push_back(memory(index));
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
