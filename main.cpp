#include "fixups.h"
#include "Parser/Parser.h"
#include "IR/IntRep.h"
#include "IR/SymbolVertex.h"
#include "Interpreter/Interpreter.h"
#include "Interpreter/Value.h"
#include <fstream>

sint32 Main(const vector<string>& args)
{
	/// Parser -> ASG -> Interpreter
	
	/// TODO: Validate programs
	///  Rule: Only cycles allowed are in ClosureDefinition from arguments or closure to returns
	/// TODO: Identifier prefixes
	/// TODO: Calculate minimal closures
	/// TODO: Calculate minimal stacks
	/// TODO: A call should never require memory allocation
	/// TODO: Correct memory management in interpreter
	/// TODO: Store minimal context in closures
	/// TODO: Fixed stacks per closure
	
	/// TODO: String support
	/// TODO: Standard library with I/O
	/// TODO: Reflection
	/// TODO: Dynamic syntax sugar system
	/// TODO: Self-interpreter
	/// TODO: Memory management in language
	/// TODO: Inline hot closures and compile to machine code
	
	/// TODO: Expand expressions between ( ) as infix math using temps
	
	wcerr << L"Simple C++ interpreter for the language using ≔ and ↦." << endl;
	wcerr << endl;
	wcerr << args << endl;
	if(args.size() < 3)
	{
		wcerr << "Usage: proglang source_file function [arguments]*" << endl;
		throw std::runtime_error("Not enough arguments.");
	}
	
	// Open
	std::wifstream input;
	input.open(encodeLocal(args[1]), std::ios_base::in);
	if(!input.good())
	{
		throw std::runtime_error("Could not open source file.");
	}
	
	// Parse file
	wcerr << L"Parsing file…" << flush;
	IntRep* ir = new IntRep();
	Parser parser(ir);
	while(input.good())
	{
		string line;
		std::getline<wchar>(input, line);
		parser.parseLine(line);
	}
	wcerr << endl;
	
	// Find function to call
	wcerr << L"Looking for " << args[2] << L"…" << flush;
	SymbolVertex* function = 0;
	foreach(symbol, ir->symbols())
		if(symbol->identifier() == args[2])
			function = symbol;
	if(!function)
		throw std::runtime_error("Could not find specified function.");
	wcerr << endl;
	
	// Parse arguments
	wcerr << L"Parsing arguments…" << flush;
	vector<Value> arguments;
	for(unsigned int i = 3; i < args.size(); i++)
	{
		arguments.push_back(parse<sint64>(args[i]));
	}
	wcerr << endl;
	
	// Execute IR
	Interpreter interpreter(ir);
	vector<Value> returns;
	returns = interpreter.evaluateCall(function, arguments);
	wcout << returns;
	
	return 0;
}
