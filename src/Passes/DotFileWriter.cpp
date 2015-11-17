#include <Passes/DotFileWriter.h>
#include <Parser/IdentifierProperty.h>
#include <Parser/ConstantProperty.h>
#include <fstream>

DotFileWriter::DotFileWriter(const string& filename)
: _out(encodeLocal(filename))
{
}

void DotFileWriter::write(const DataFlowGraph& dfg)
{
	_out << "digraph {" << endl;
	for(std::shared_ptr<Node> node: dfg.nodes())
		write(node);
	_out << "}" << endl;
}

void DotFileWriter::write(std::shared_ptr<Node> node)
{
	_out << nodeId(node);
	_out << " [";
	if(node->type() == NodeType::Call) {
		_out << "shape=circle";
		const std::shared_ptr<Edge> label = node->in(0);
		if(label->has<IdentifierProperty>())
			_out << " label=\"" << label->get<IdentifierProperty>().value() << "\"";
		else
			_out << " label=\"( )\"";
		_out << "];" << endl;
	} else if(node->type() == NodeType::Closure) {
		_out << "shape=doublecircle";
		const std::shared_ptr<Edge> label = node->out(0);
		if(label->has<IdentifierProperty>())
			_out << " label=\"" << label->get<IdentifierProperty>().value() << "\"";
		else
			_out << " label=\"( )\"";
		_out << "];" << endl;
	}
	for(auto edge: node->out())
		write(edge);
}

void DotFileWriter::write(std::shared_ptr<Edge> edge)
{
	// Todo: Create source node for constants
	// Todo: Identifers instead of numbers
	for(auto sink: edge->sinks()) {
		_out << nodeId(edge->source()) << " -> ";
		_out << nodeId(sink.target.lock()) << " [";
		_out << "headlabel=" << edge->index() << ",";
		_out << "taillabel=" << sink.index << ",samehead";
		_out << "]" << endl;
	}
}

string DotFileWriter::nodeId(std::shared_ptr<Node> node)
{
	return toString(reinterpret_cast<uint64>(node.get()));
}
