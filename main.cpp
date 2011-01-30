#include<fixups.h>
#include"Interpreter.h"

using namespace std;

sint32 Main(const std::vector<wstring>& args)
{
	wcout << L"Simple C++ interpreter for the language using ≔ and ↦." << endl;
	wcout << endl;
	Interpreter interpreter;
	wstring line;
	while(wcin.good())
	{
		wcout << L"> ";
		getline<wchar>(wcin, line);
		interpreter.processLine(line);
	}
	wcout << endl;
	if(wcin.fail() && !wcin.eof())
	{
		throw runtime_error("Could not read input.");
	}
	return 0;
}
