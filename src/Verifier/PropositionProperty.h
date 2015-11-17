#pragma once
#include <DFG/PropertyMap.h>
class Edge;

class PropositionProperty: public PropertyMap::Property
{
public:
	PropositionProperty();
	PropositionProperty(const PropositionProperty& copy);
	virtual ~PropositionProperty();
	PropositionProperty& operator=(const PropositionProperty& copy);
	virtual void print(std::wostream& out) const override;
	
	const std::vector<std::shared_ptr<Edge>>& preconditions() const { return _preconditions; }
	const std::vector<std::shared_ptr<Edge>>& axioms() const { return _axioms; }
	const std::vector<std::shared_ptr<Edge>>& assertions() const { return _assertions; }
	const std::vector<std::shared_ptr<Edge>>& postconditions() const { return _postconditions; }
	
	void precondition(std::shared_ptr<Edge> value) { _preconditions.push_back(value); }
	void axiom(std::shared_ptr<Edge> value) { _axioms.push_back(value); }
	void assertion(std::shared_ptr<Edge> value) { _assertions.push_back(value); }
	void postcondition(std::shared_ptr<Edge> value) { _postconditions.push_back(value); }
	
	
protected:
	std::vector<std::shared_ptr<Edge>> _preconditions;
	std::vector<std::shared_ptr<Edge>> _axioms;
	std::vector<std::shared_ptr<Edge>> _assertions;
	std::vector<std::shared_ptr<Edge>> _postconditions;
};
