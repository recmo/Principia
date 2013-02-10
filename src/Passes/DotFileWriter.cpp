#include "Passes/DotFileWriter.h"
#include "Parser/IdentifierProperty.h"
#include <fstream>
#include <Parser/ConstantProperty.h>

const bool showEdges = false;
const bool showClosureFunc = false;

/// TODO: Graph kinds:
/// - Full graph with edge nodes
/// - Reduced graph by contracting edge nodes
/// - Reduced graph by contracting edge nodes and call nodes

/// TODO: Graph only including closures, i.e. a call graph
/// note that there are two edge kinds: ‘calls’ and “closes over”

DotFileWriter::DotFileWriter(const string& filename)
: _contractionMode(None)
, _out(encodeLocal(filename))
{
	_out << "digraph {" << endl;
}

DotFileWriter::~DotFileWriter()
{
	_out << "}" << endl;
}

void DotFileWriter::write(const DataFlowGraph* dfg)
{
	for(const Node* node: dfg->nodes())
		write(node);
}

void DotFileWriter::write(const Node* node)
{
	if(_contractionMode == Calls && node->type() != NodeType::Closure)
		return;
	
	_out << objectId(&node);
	_out << " [";
	if(node->type() == NodeType::Call) {
		_out << "shape=circle";
		const Edge* label = node->in(0);
		if(label->has<IdentifierProperty>())
			_out << " label=\"" << label->get<IdentifierProperty>().value() << "\"";
		else
			_out << " label=\"( )\"";
		_out << "];" << endl;
	} else if(node->type() == NodeType::Closure) {
		_out << "shape=doublecircle";
		const Edge* label = node->out(0);
		if(label->has<IdentifierProperty>())
			_out << " label=\"" << label->get<IdentifierProperty>().value() << "\"";
		else
			_out << " label=\"( )\"";
		_out << "];" << endl;
	}
	if(_contractionMode == None)
	{
		for(int i = 0; i < node->outArity(); ++i) {
			write(node->out(i));
			_out << objectId(&node) << " -> " << objectId(node->out(i)) << " [";
			_out << " label=" << i;
			if(i == 0 && node->type() == NodeType::Closure)
				_out << " style=dotted";
			_out << "];" << endl;
		}
		for(int i = 0; i < node->inArity(); ++i) {
			write(node->in(i));
			_out << objectId(node->in(i)) << " -> " << objectId(&node) << " [";
			_out << " label=" << i;
			
			wcerr << "Node: " << uint64(node) << endl;
			wcerr << "Node: " << node << endl;
			wcerr << "i = " << i << endl;
			wcerr << "In: " << node->in(i) << endl;
			node->in(i)->print(wcerr);
			wcerr << endl;
			wcerr << "In: " << uint64(node->in(i)->source()) << endl;
			wcerr << "In: " << node->in(i)->source() << endl;
			
			assert(node->in(i));
			
			if(node->in(i)->source() && node->in(i)->source()->type() == NodeType::Closure && node->in(i)->source()->out(0) == node->in(i))
				_out << " style=dotted";
			_out << "];" << endl;
		}
	}
	else if(_contractionMode == Edges)
	{
		for(int i = 0; i < node->outArity(); ++i) {
			const Edge* out = node->out(i);
			bool dotted = false;
			if(!showClosureFunc && node->type() == NodeType::Closure && i == 0)
				dotted = true;
			for(int j = 0; j < out->sinks().size(); ++j) {
				_out << objectId(&node) << " -> " << objectId(out->sinks()[j]);
				if(dotted) _out << " [style=dotted]";
				_out << endl;
			}
		}
	}
	else if(_contractionMode == Calls)
	{
		set<const Node*> out = contractCalls(node, false);
		out.erase(node);
		for(auto it =out.begin(); it != out.end(); ++it)
			_out << objectId(node) << " -> " << objectId(*it) << endl;
		out = contractCalls(node, true);
		for(auto it =out.begin(); it != out.end(); ++it)
			_out << objectId(node) << " -> " << objectId(*it) << " [style=dotted]" << endl;
	}
}

void DotFileWriter::write(const Edge* edge)
{
	if(_done.find(edge) != _done.end())
		return;
	_done.insert(edge);
	_out << objectId(edge);
	_out << " [shape=diamond";
	if(edge->has<IdentifierProperty>())
		_out << " label=\"" << edge->get<IdentifierProperty>().value() << "\"";
	else if(edge->has<ConstantProperty>())
		/// TODO: Escape string values
		_out << " label=\"" << edge->get<ConstantProperty>().value() << "\"";
	else
		_out << " label=\"( )\"";
	_out << "];" << endl;
	return;
	for(int i = 0; i < edge->sinks().size(); ++i)
		_out << objectId(edge) << " -> " << objectId(edge->sinks()[i]) << " [style=dotted]" << endl;
	_out << objectId(edge->source()) << " -> " << objectId(edge) << " [style=dashed]" << endl;
}

string DotFileWriter::objectId(const void* object)
{
	std::wostringstream ss;
	ss << L"\"" << std::hex << object << L"\"";
	return ss.str();
}

std::set<const Node*> DotFileWriter::contractEdges(const Node* node)
{
	set<const Node*> contracted;
	for(int i = 0; i < node->outArity(); ++i) {
		const Edge* out = node->out(i);
		for(int j = 0; j < out->sinks().size(); ++j) {
			contracted.insert(out->sinks()[j]);
		}
	}
	return contracted;
}

std::set<const Node*> DotFileWriter::contractCalls(const Node* node, bool functional)
{
	assert(node->type() == NodeType::Closure);
	
	// Find all nodes reached from this one upto closure nodes
	set<const Node*> reached;
	if(functional) {
			const Edge* out = node->out(0);
			for(int j = 0; j < out->sinks().size(); ++j)
				contractCalls(out->sinks()[j], reached);
	} else
		for(int i = 1; i < node->outArity(); ++i) {
			const Edge* out = node->out(i);
			for(int j = 0; j < out->sinks().size(); ++j)
				contractCalls(out->sinks()[j], reached);
		}
	
	// Take the subset consisting only of the closures
	set<const Node*> contracted;
	for(auto it = reached.begin(); it != reached.end(); ++it)
		if((*it)->type() == NodeType::Closure)
			contracted.insert(*it);
	return contracted;
}

void DotFileWriter::contractCalls(const Node* node, std::set<const Node*>& visited)
{
	if(visited.find(node) != visited.end())
		return;
	visited.insert(node);
	if(node->type() == NodeType::Closure)
		return;
	for(int i = 0; i < node->outArity(); ++i) {
		const Edge* out = node->out(i);
		for(int j = 0; j < out->sinks().size(); ++j) {
			contractCalls(out->sinks()[j], visited);
		}
	}
}


