#pragma once
#include <DFG/PropertyMap.h>
#include <DFG/OutPort.h>

class PropositionProperty: public PropertyMap::Property
{
public:
	virtual void print(std::wostream& out) const override;
	
	const std::vector<std::shared_ptr<OutPort>>& preconditions() const
	{ return _preconditions; }
	const std::vector<std::shared_ptr<OutPort>>& axioms() const
	{ return _axioms; }
	const std::vector<std::shared_ptr<OutPort>>& assertions() const
	{ return _assertions; }
	const std::vector<std::shared_ptr<OutPort>>& postconditions() const
	{ return _postconditions; }
	
	void precondition(std::shared_ptr<OutPort> value) { _preconditions.push_back(value); }
	void axiom(std::shared_ptr<OutPort> value) { _axioms.push_back(value); }
	void assertion(std::shared_ptr<OutPort> value) { _assertions.push_back(value); }
	void postcondition(std::shared_ptr<OutPort> value) { _postconditions.push_back(value); }
	
protected:
	std::vector<std::shared_ptr<OutPort>> _preconditions;
	std::vector<std::shared_ptr<OutPort>> _axioms;
	std::vector<std::shared_ptr<OutPort>> _assertions;
	std::vector<std::shared_ptr<OutPort>> _postconditions;
};
