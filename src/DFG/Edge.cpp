#include "DFG/Edge.h"
#include "Node.h"
#include "Parser/SourceProperty.h"
#include "Parser/IdentifierProperty.h"
#include <Parser/ConstantProperty.h>

Edge::Edge(Node* source)
: PropertyMap()
, _source(source)
, _sinks()
{
}

Edge::~Edge()
{
	for(auto it = _sinks.begin(); it != _sinks.end(); ++it)
		(*it)->forgetEdge(this);
}

uint Edge::sourceIndex() const
{
	assert(_source);
	uint index = _source->outIndexOf(this);
	assert(index < _source->outArity());
	return index;
}

void Edge::addSink(Node* node)
{
	for(auto it = _sinks.begin(); it != _sinks.end(); ++it)
		if(*it == node)
			return;
	_sinks.push_back(node);
}

void Edge::delSink(const Node* node)
{
	assert(node);
	auto it = _sinks.begin();
	for(; it != _sinks.end(); ++it) {
		Node* n = *it;
		assert(n);
		if(n == node)
			break;
	}
	assert(it != _sinks.end());
	_sinks.erase(it);
}

void Edge::replaceWith(Edge* edge)
{
	for(auto it = _sinks.begin(); it != _sinks.end(); ++it)
		(*it)->replaceEdge(this, edge);
	_sinks.empty();
}

void Edge::print(std::wostream& out) const
{
	if(has<IdentifierProperty>())
		out << get<IdentifierProperty>().value();
	else if(has<ConstantProperty>())
		out << get<ConstantProperty>().value();
	else if(has<SourceProperty>()) {
		SourceProperty sp = get<SourceProperty>();
		out << L"<" << sp.fromLine();
		out << L":" << sp.fromColumn();
		out << L">";
	} else {
		out << L"<" << uint64(this) << ">";
	}
}

bool Edge::isFunction() const
{
	if(has<ConstantProperty>() && get<ConstantProperty>().type() == Value::Builtin)
		return true;
	return _source->type() == NodeType::Closure && _source->outArity() >= 1 && _source->out(0) == this;
}
