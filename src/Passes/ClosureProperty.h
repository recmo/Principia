#pragma once
#include <DFG/OutPort.h>
#include <DFG/PropertyMap.h>
#include <vector>

class ClosureProperty: public PropertyMap::Property
{
public:
	typedef std::shared_ptr<OutPort> OutPortPtr;
	typedef std::vector<OutPortPtr> ClosureSet;
	
	ClosureProperty(const ClosureSet& closureSet)
	: _closureSet(closureSet) { }
	virtual void print(std::wostream& out) const override
	{ out << closureSet(); }
	
	const ClosureSet& closureSet() const { return _closureSet; }
	ClosureSet& closureSet() { return _closureSet; }
	
protected:
	ClosureSet _closureSet;
};
