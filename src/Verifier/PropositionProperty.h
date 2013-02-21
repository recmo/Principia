#pragma oncepos
#include "fixups.h"
#include "DFG/PropertyType.h"
#include "DFG/Property.h"
class Edge;

class PropositionProperty: public Property
{
public:
	PropositionProperty();
	PropositionProperty(const PropositionProperty& copy);
	virtual ~PropositionProperty();
	PropositionProperty& operator=(const PropositionProperty& copy);
	
	static PropertyType classType;
	virtual PropertyType type() const { return classType; }
	virtual PropositionProperty* clone() const { return new PropositionProperty(*this); }
	
	const std::vector<Edge*>& preconditions() const { return _preconditions; }
	const std::vector<Edge*>& axioms() const { return _axioms; }
	const std::vector<Edge*>& assertions() const { return _assertions; }
	const std::vector<Edge*>& postconditions() const { return _postconditions; }
	
	void precondition(Edge* value) { _preconditions.push_back(value); }
	void axiom(Edge* value) { _axioms.push_back(value); }
	void assertion(Edge* value) { _assertions.push_back(value); }
	void postcondition(Edge* value) { _postconditions.push_back(value); }
	
	virtual void print(std::wostream& out) const;
	
protected:
	std::vector<Edge*> _preconditions;
	std::vector<Edge*> _axioms;
	std::vector<Edge*> _assertions;
	std::vector<Edge*> _postconditions;
};
