#pragma once
#include <DFG/Port.h>
#include <memory>
class Node;
class InPort;

class OutPort: public Port, public std::enable_shared_from_this<OutPort>
{
public:
	typedef std::set<std::weak_ptr<InPort>, std::owner_less<std::weak_ptr<InPort>>> SinkSet;
	
	OutPort(Node& parent, uint index): Port(parent, index) { }
	
	const SinkSet& sinks() const { return _sinks; }
	
protected:
	friend class InPort;
	void add_sink(InPort& sink);
	void del_sink(InPort& sink);
	
private:
	SinkSet _sinks;
};
