#pragma once
#include <fixups.h>

class MetaMathFrame
{

protected:
	vector<string> _variables;
	vector<string> _types; // Floating hypothesis
	/// @todo Distinct variable specifiers
	vector<MetaMathStatement*> _hypothesis;
	MetaMathStatement* _result;
};
