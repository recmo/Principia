#pragma once
#include <Unicode/string.h>
#include <vector>
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
	std::vector<MetaMathScope*> _scopeStack;
	
	MetaMathScope* currentScope();
	void pushScope();
	void popScope();
	void handleStatement(const string& label, const string& operation,
		const std::vector<string>& arguments, const std::vector<string>& proof);
	
	static string improveToken(const string& token);
};
