#pragma once
#include "fixups.h"
#include <DFG/DataFlowGraph.h>
#include <fstream>

class DotFileWriter
{
public:
	DotFileWriter(const string& filename);
	DotFileWriter(std::wostream& out);
	~DotFileWriter();
	
	enum ContractionMode {
		None,
		Edges,
		Calls
	};
	
	ContractionMode contractionMode() const { return _contractionMode; }
	DotFileWriter& contractionMode(const ContractionMode& value) { _contractionMode = value; return *this; }
	
	void write(const DataFlowGraph& dfg);
	void write(const Node& node);
	void write(const Edge& edge);
	
	set<const Node*> contractEdges(const Node*);
	set<const Node*> contractCalls(const Node* node, bool functional);
	void contractCalls(const Node* node, set<const Node*>& reached);
	
	
private:
	ContractionMode _contractionMode;
	
	std::wofstream _out;
	string objectId(const void* object);
	set<const Edge*> _done;
};

