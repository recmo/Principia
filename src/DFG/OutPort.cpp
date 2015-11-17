#include "OutPort.h"
#include <DFG/InPort.h>
#include <Utilities/containers.h>
#include <Utilities/assert.h>

void OutPort::add_sink(InPort& sink)
{
	std::weak_ptr<InPort> ref = sink.shared_from_this();
	assert(!contains(_sinks, ref));
	_sinks.insert(ref);
	assert(contains(_sinks, ref));
}

void OutPort::del_sink(InPort& sink)
{
	std::weak_ptr<InPort> ref = sink.shared_from_this();
	assert(contains(_sinks, ref));
	_sinks.erase(ref);
	assert(!contains(_sinks, ref));
}
