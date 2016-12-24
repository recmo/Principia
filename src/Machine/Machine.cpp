#include "Machine.h"
namespace Machine {


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
		
		//
		// Symbolic execution on stack machine
		//
		
		// Create empty stack, list of imports and list of constants
		std::vector<Compile::Symbol> stack;
		std::vector<std::shared_ptr<value_t>> constants;
		
		// Push imports on the stack
		for(const auto& import: compile_func.imports) {
			stack.push_back(import.first);
			std::shared_ptr<value_t> value = std::make_shared<value_t>();
			value->import = import.second;
			constants.push_back(value);
		}
		
		// Push constants on the stack
		for(const auto& constant: compile_func.constants) {
			stack.push_back(constant.first);
			std::shared_ptr<value_t> value = std::make_shared<value_t>();
			value->constant = constant.second;
			constants.push_back(value);
		}
		
		// Push closure arguments on the stack
		std::copy(compile_func.closure.begin(), compile_func.closure.end(), std::back_inserter(stack));
		
		// Push call arguments on the stack
		std::copy(compile_func.arguments.begin(), compile_func.arguments.end(), std::back_inserter(stack));
		
		// Push allocations on the stack
		for(uint fid: compile_func.allocations) {
			stack.push_back(std::make_pair(fid, 0));
		}
		
		//
		// Turn allocation and tail call symbols into stack indices
		//
		
		// Extract a stack map from symbol to stack index
		std::map<Parser::Symbol, uint16_t> symbol_map;
		for(uint i = 0; i < stack.size(); ++i) {
			if(symbol_map.find(stack[i]) != symbol_map.end()) {
				std::wcerr << L"Duplicate symbol:" << stack[i] << L"\n";
				throw "Duplicate symbol";
			}
			symbol_map[stack[i]] = i;
		}
		// std::wcerr << symbol_map << std::endl;
		
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
		functions.push_back(std::move(machine_func));
	}
	
	// Make sure we have a valid main
	assert(main_index != -1);
	const function_t& main = functions[main_index];
	assert(main.closures == 0);
	assert(main.arguments == 1);
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
			
			// Virtual stack
			std::function<std::shared_ptr<value_t>(uint)> stack = [&](uint index) {
				if(index < func.constants.size()) {
					return func.constants[index];
				}
				index -= func.constants.size();
				if(index < func.closures) {
					return closure->values[index];
				}
				index -= func.closures;
				if(index < func.arguments) {
					return arguments[index];
				}
				index -= func.arguments;
				assert(index < allocs.size());
				return allocs[index];
			};
			
			// Execute alloc instructions
			for(const alloc_instruction_t& inst: func.allocs) {
				std::shared_ptr<value_t> closure = std::make_shared<value_t>();
				closure->function_index = inst.function_index;
				for(uint16_t index: inst.closure) {
					closure->values.push_back(stack(index));
				}
				allocs.push_back(std::move(closure));
			}
			
			// Load call instruction
			assert(func.call.size() >= 1);
			std::vector<std::shared_ptr<value_t>> new_arguments;
			new_arguments.reserve(func.call.size() - 1);
			for(uint i = 1; i < func.call.size(); ++i) {
				new_arguments.push_back(stack(func.call[i]));
			}
			closure = stack(func.call[0]);
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
