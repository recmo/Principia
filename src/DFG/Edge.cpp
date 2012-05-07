#include "DFG/Edge.h"
#include "Node.h"
#include "Parser/SourceProperty.h"
#include "Parser/IdentifierProperty.h"

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

void Edge::addSink(Node* node)
{
	for(auto it = _sinks.begin(); it != _sinks.end(); ++it)
		if(*it == node)
			return;
	_sinks.push_back(node);
}

void Edge::delSink(const Node* node)
{
	assert(node != 0);
	auto it = _sinks.begin();
	for(; it != _sinks.end(); ++it)
	{
		Node* n = *it;
		assert(n != 0);
		wcerr << n << endl;
		wcerr << node << endl;
		if(n != node)
			continue;
	}
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
	else if(has<SourceProperty>()) {
		SourceProperty sp = get<SourceProperty>();
		out << L"<" << sp.fromLine();
		out << L":" << sp.fromColumn();
		out << L">";
	} else
		out << L"<anonymous>";
}

bool Edge::isFunction() const
{
	return _source->type() == NodeType::Closure && _source->outArrity() >= 1 && _source->out(0) == this;
}
