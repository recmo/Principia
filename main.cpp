#include<fixups.h>
#include"Interpreter.h"

using namespace std;

sint32 Main(const std::vector<wstring>& args)
{
	/// TODO: Memory management in interpreter
	/// TODO: Static analysis
	/// TODO: Store minimal context in closures
	
	/// TODO: String support
	/// TODO: Standard library with I/O
	/// TODO: Reflection
	/// TODO: Dynamic syntax sugar system
	/// TODO: Self-interpreter
	/// TODO: Memory management in language
	/// TODO: Inline hot closures and compile to machine code
	
	wcout << L"Simple C++ interpreter for the language using ≔ and ↦." << endl;
	wcout << endl;
	Interpreter interpreter;
	wstring line;
	while(wcin.good())
	{
		wcout << L"> ";
		getline<wchar>(wcin, line);
		try
		{
			interpreter.processLine(line);
		}
		catch(exception e)
		{
			wcerr << L"Exception: " << e.what() << endl;
		}
	}
	wcout << endl;
	if(wcin.fail() && !wcin.eof())
	{
		throw runtime_error("Could not read input.");
	}
	return 0;
}
