#pragma once
#include <DFG/Port.h>
#include <Utilities/enable_auto_shared.h>
#include <memory>
#include <iostream>
class Node;
class InPort;

class OutPort: public Port, public enable_auto_shared<OutPort>
{
public:
	typedef std::weak_ptr<InPort> Sink;
	typedef std::set<Sink, std::owner_less<Sink>> SinkSet;
	
	OutPort(Node& parent, uint index): Port(parent, index) { }
	
	const SinkSet& sinks() const { return _sinks; }
	
protected:
	friend class InPort;
	void add_sink(InPort& sink);
	void del_sink(InPort& sink);
	
private:
	SinkSet _sinks;
};

std::wostream& operator<<(std::wostream& out, const OutPort& value);

std::ostream& operator<<(std::ostream& out, const OutPort& value);
