#include "DFG/Node.h"
#include "DFG/Edge.h"
#include <Parser/IdentifierProperty.h>
#include <Parser/SourceProperty.h>
#include <set>
using namespace std;

Node::Node(NodeType type, int inArrity, int outArity)
: PropertyMap()
, _type(type)
, _inArrity(inArrity)
, _outArrity(outArity)
, _incomming(0)
, _outgoing(0)
{
	assert(type.isValid());
	assert(inArrity >= (type == NodeType::Call) ? 1 : 0);
	assert(outArity >= (type == NodeType::Closure)? 1 : 0);
	_incomming = new Edge*[_inArrity];
	_outgoing = new Edge[_outArrity];
	for(int i = 0; i < _inArrity; ++i)
		_incomming[i] = 0;
	for(int i = 0; i < _outArrity; ++i)
		_outgoing[i]._source = this;
}

Node::~Node()
{
	for(int i = 0; i < _inArrity; ++i) {
		if(_incomming[i] == 0)
			continue;
		_incomming[i]->_source = this;
	}
	delete[] _incomming;
	delete[] _outgoing;
}

void Node::forgetEdge(const Edge* edge)
{
	for(int i = 0; i < _inArrity; ++i)
		if(_incomming[i] == edge)
			_incomming[i] = 0;
}

void Node::connect(int index, Edge* edge)
{
	assert(edge != 0);
	assert(index >= 0 && index < _inArrity);
	if(_incomming[index] != 0)
		_incomming[index]->delSink(this);
	_incomming[index] = edge;
	edge->addSink(this);
}

void Node::replaceEdge(const Edge* from, Edge* to)
{
	assert(from != 0);
	assert(to != 0);
	assert(from != to);
	bool foundsomething = false;
	for(int i = 0; i < _inArrity; ++i)
		if(foundsomething  |= _incomming[i] == from)
			_incomming[i] = to;
	to->addSink(this);
	assert(foundsomething);
}

void Node::print(std::wostream& out) const
{
	if(type() == NodeType::Call)
		out << L"≔";
	if(has<IdentifierProperty>())
		out << get<IdentifierProperty>().value();
	else if(has<SourceProperty>()) {
		SourceProperty sp = get<SourceProperty>();
		out << L"<" << sp.fromLine();
		out << L":" << sp.fromColumn();
		out << L">";
	} else
		out << L"<anonymous>";
	if(type() == NodeType::Closure)
		out << L"↦";
}

std::set<Node*> Node::outNodes(bool ignoreFunctional)
{
	std::set<Node*> outNodes;
	bool skipFirst = ignoreFunctional && type() == NodeType::Closure;
	for(int i = (skipFirst) ? 1 : 0; i < outArrity(); ++i)
		for(int j = 0; j < out(i)->sinks().size(); ++j)
			outNodes.insert(out(i)->sinks()[j]);
	return outNodes;
}

std::set<Node*> Node::outClosures(bool ignoreFunctional)
{

}






