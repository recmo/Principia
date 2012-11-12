#pragma once
#include <fixups.h>
using std::map;
class MetaMathScope;
class MetaMathStatement;

class MetaMathVerfier
{
public:
	MetaMathVerfier();
	~MetaMathVerfier();
	
	bool verify();
	
protected:
	MetaMathStatement* _statement;
	vector<vector<string>> _stack;
	map<string, vector<string>> _substitution;
};
