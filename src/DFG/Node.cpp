#include "DFG/Node.h"
#include "DFG/Edge.h"
#include <Parser/IdentifierProperty.h>
#include <Parser/SourceProperty.h>
#include <set>
using namespace std;

Node::Node(NodeType type, int inArity, int outArity)
: PropertyMap()
, _type(type)
, _inArity(inArity)
, _outArity(outArity)
, _incomming(new Edge*[inArity])
, _outgoing(new Edge[outArity])
{
	assert(type.isValid());
	assert(inArity >= (type == NodeType::Call) ? 1 : 0);
	assert(outArity >= (type == NodeType::Closure)? 1 : 0);
	for(uint i = 0; i < _inArity; ++i)
		_incomming[i] = nullptr;
	for(uint i = 0; i < _outArity; ++i)
		_outgoing[i]._source = this;
	for(uint i = 0; i < _outArity; ++i)
		assert(_outgoing[i].sourceIndex() == i);
}

Node::~Node()
{
	for(uint i = 0; i < _inArity; ++i)
		_incomming[i]->delSink(this);
	delete[] _incomming;
	delete[] _outgoing;
}

void Node::forgetEdge(const Edge* edge)
{
	for(uint i = 0; i < _inArity; ++i)
		if(_incomming[i] == edge)
			_incomming[i] = nullptr;
}

void Node::connect(uint index, Edge* edge)
{
	assert(edge);
	assert(index < _inArity);
	if(_incomming[index])
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
	for(uint i = 0; i < _inArity; ++i){
		if(_incomming[i] == from) {
			_incomming[i] = to;
			foundsomething = true;
		}
	}
	to->addSink(this);
	wcerr << "Node " << this << " did not contain " << from << endl;
	// assert(foundsomething);
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
	} else {
		if(type() == NodeType::Call && inArity() > 0 && in(0)->has<IdentifierProperty>())
			out << L"<" << in(0)->get<IdentifierProperty>().value() << L">";
		else if(type() == NodeType::Closure && outArity() > 0 && _outgoing[0].has<IdentifierProperty>())
			out << L"<" << _outgoing[0].get<IdentifierProperty>().value() << L">";
		else 
			out << L"<anonymous>";
	}
	if(type() == NodeType::Closure)
		out << L"↦";
}

std::set<Node*> Node::outNodes(bool ignoreFunctional)
{
	std::set<Node*> outNodes;
	bool skipFirst = ignoreFunctional && type() == NodeType::Closure;
	for(uint i = (skipFirst) ? 1 : 0; i < outArity(); ++i)
		for(uint j = 0; j < out(i)->sinks().size(); ++j)
			outNodes.insert(out(i)->sinks()[j]);
	return outNodes;
}

std::set<Node*> Node::outClosures(bool ignoreFunctional)
{
	throw "Unimplemted...";
}

std::vector<const Edge*> Node::constIn() const
{
	vector<const Edge*> result;
	for(uint i = 0; i < inArity(); ++i)
		result.push_back(in(i));
	return result;
}

std::vector<Edge*> Node::in()
{
	vector<Edge*> result;
	for(uint i = 0; i < inArity(); ++i)
		result.push_back(in(i));
	return result;
}

std::vector<const Edge*> Node::constOut() const
{
	vector<const Edge*> result;
	for(uint i = 0; i < outArity(); ++i)
		result.push_back(out(i));
	return result;
}

std::vector<Edge*> Node::out()
{
	vector<Edge*> result;
	for(uint i = 0; i < outArity(); ++i)
		result.push_back(out(i));
	return result;
}

uint Node::inIndexOf(const Edge* edge) const
{
	for(uint i = 0; i < _inArity; ++i)
		if(_incomming[i] == edge)
			return i;
	return -1;
}

uint Node::outIndexOf(const Edge* edge) const
{
	uint i = edge - _outgoing;
	if(i >= _outArity)
		i = -1;
	assert(out(i) == edge);
	return i;
}

void Node::check() const
{
	for(uint i = 0; i < _outArity; ++i)
		assert(_outgoing[i]._source == this);
}
