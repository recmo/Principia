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
	
	const std::vector<Edge*>& preconditions() const { return _preconditions; }
	const std::vector<Edge*>& axioms() const { return _axioms; }
	const std::vector<Edge*>& assertions() const { return _assertions; }
	const std::vector<Edge*>& postconditions() const { return _postconditions; }
	
	void precondition(Edge* value) { _preconditions.push_back(value); }
	void axiom(Edge* value) { _axioms.push_back(value); }
	void assertion(Edge* value) { _assertions.push_back(value); }
	void postcondition(Edge* value) { _postconditions.push_back(value); }
	
	
protected:
	std::vector<Edge*> _preconditions;
	std::vector<Edge*> _axioms;
	std::vector<Edge*> _assertions;
	std::vector<Edge*> _postconditions;
};
