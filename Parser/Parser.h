#pragma once
#include "fixups.h"
#include <IR/IntRep.h>

class Parser
{
public:
	Parser(IntRep* program);
	~Parser();
	
	void parseFile(const string& filename);
	void parseStream(std::wistream& stream);
	void parseLine(const string& line);
	
private:
	IntRep* _program;
	SymbolVertex* symbolFromIdentifier(const string& identifier, bool autoCreate = false);
	
};
