#pragma once
#include <DFG/Port.h>
#include <memory>
class Node;
class InPort;

class OutPort: public Port, public std::enable_shared_from_this<OutPort>
{
public:
	OutPort(Node& parent, uint index): Port(parent, index) { }
	
	std::set<std::weak_ptr<InPort>, std::owner_less<std::weak_ptr<InPort>>>&
	sinks() { return _sinks; }
	const std::set<std::weak_ptr<InPort>, std::owner_less<std::weak_ptr<InPort>>>&
	sinks() const { return _sinks; }
	
protected:
	friend class InPort;
	void add_sink(InPort& sink);
	void del_sink(InPort& sink);
	
private:
	std::set<std::weak_ptr<InPort>, std::owner_less<std::weak_ptr<InPort>>> _sinks;
};
