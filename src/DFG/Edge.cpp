#include "Edge.h"
#include <DFG/Node.h>
#include <Parser/SourceProperty.h>
#include <Parser/IdentifierProperty.h>
#include <Parser/ConstantProperty.h>

Edge::Edge(std::shared_ptr<Node> source, uint index)
: _source(std::move(source))
, _index(index)
{
	assert(source != nullptr);
}

std::shared_ptr<Node> Edge::source() const
{
	std::shared_ptr<Node> source = _source.lock();
	assert(source != nullptr);
	return source;
}

void Edge::replaceWith(std::shared_ptr<Edge> edge)
{
	const std::set<Sink> sinks = _sinks;
	for(Sink sink: sinks) {
		std::shared_ptr<Node> target = sink.target.lock();
		if(target == nullptr)
			continue;
		target->connect(sink.index, edge);
	}
	assert(_sinks.size() == 0);
}

std::wostream& operator<<(std::wostream& out, const Edge& edge)
{
	if(edge.has<IdentifierProperty>() && edge.get<IdentifierProperty>().value() != L"·")
		out << edge.get<IdentifierProperty>().value();
	else if(edge.has<ConstantProperty>())
		out << edge.get<ConstantProperty>().value();
	else if(edge.has<SourceProperty>()) {
		SourceProperty sp = edge.get<SourceProperty>();
		sp.print(out);
	} else {
		out << L"<" << reinterpret_cast<uint64>(&edge) << ">";
	}
	return out;
}

bool Edge::isFunction() const
{
	if(has<ConstantProperty>()
		&& get<ConstantProperty>().value().kind == Value::Builtin) {
		return true;
	}
	std::shared_ptr<Node> s = source();
	return index() == 0 && s != nullptr && s->type() == NodeType::Closure;
}

bool Edge::Sink::operator==(const Edge::Sink& other) const
{
	if(index != other.index)
		return false;
	std::shared_ptr<Node> ours = target.lock();
	std::shared_ptr<Node> theirs = other.target.lock();
	return ours == theirs;
}

bool Edge::Sink::operator<(const Edge::Sink& other) const
{
	if(index != other.index)
		return index < other.index;
	std::shared_ptr<Node> ours = target.lock();
	std::shared_ptr<Node> theirs = other.target.lock();
	return ours.get() < theirs.get();
}
