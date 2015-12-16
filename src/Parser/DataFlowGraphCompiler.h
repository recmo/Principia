#pragma once
#include <iostream>
#include <string>
#include <Unicode/string.h>
class DataFlowGraph;
class ParseTree;

class DataFlowGraphCompiler
{
public:
	static DataFlowGraph* compileFile(const string& filename);
	static DataFlowGraph* compile(std::istream& stream);
	static DataFlowGraph* compileString(const std::string& bytes);
	static DataFlowGraph* compileString(const string& contents);
	
private:
	static DataFlowGraph* compile(ParseTree* tree);
};
