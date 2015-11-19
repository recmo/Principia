#pragma once
#include <DFG/DataFlowGraph.h>
#include <fstream>
#include <memory>

class DotFileWriter
{
public:
	DotFileWriter(const string& filename);
	
	void write(const DataFlowGraph& dfg);
	void write(std::shared_ptr<Node> node);
	void write(std::shared_ptr<Edge> edge);
	
private:
	static string nodeId(std::shared_ptr<Node> node);
	std::wofstream _out;
};
