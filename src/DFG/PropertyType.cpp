#include "DFG/PropertyType.h"

string PropertyType::toString() const
{
	switch(value) {
		case PropertyType::Identifier: return L"Identifier";
		case PropertyType::Source: return L"Source";
		case PropertyType::Constant: return L"Constant";
		case PropertyType::Closure: return L"Closure";
		case PropertyType::Order: return L"Order";
		case PropertyType::Stack: return L"Stack";
		case PropertyType::ReturnStack: return L"ReturnStack";
		case PropertyType::Native: return L"Native";
		case PropertyType::StackVM: return L"StackVM";
		case PropertyType::Escape: return L"Escape";
		case PropertyType::Proposition: return L"Proposition";
		default: return L"Illegal value";
	}
}
