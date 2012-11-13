#pragma once
#include <fixups.h>
class MetaMathScope;

class MetaMathParser
{
public:
	MetaMathParser(const string& filename);
	~MetaMathParser();
	
	void parse();
	
private:
	string _filename;
	MetaMathScope* _globalScope;
	vector<MetaMathScope*> _scopeStack;
	
	MetaMathScope* currentScope();
	void pushScope();
	void popScope();
	void handleStatement(const string& label, const string& operation, const vector<string>& arguments, const vector<string>& proof);
	
	static string improveToken(const string& token);
};
