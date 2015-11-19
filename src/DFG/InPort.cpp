#include "InPort.h"
#include <DFG/OutPort.h>
#include <DFG/ConstantProperty.h>
#include <Utilities/exceptions.h>

void InPort::clear()
{
	if(std::shared_ptr<OutPort> old = _source.lock())
		old->del_sink(*this);
	_source.reset();
}

InPort& InPort::operator<<(OutPort& port)
{
	clear();
	_source = port.shared_from_this();
	port.add_sink(*this);
	return *this;
}

InPort& InPort::operator<<(const Value& value)
{
	clear();
	set(ConstantProperty{value});
	return *this;
}

std::wostream& operator<<(std::wostream& out, const InPort& value)
{
	return out << L"↓";
}
