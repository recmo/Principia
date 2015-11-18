#include "PropositionProperty.h"

void PropositionProperty::print(std::wostream& out) const
{
	out << _preconditions << _axioms << _assertions << _postconditions;
}
