#include "DFG/PropertyType.h"

std::wostream& operator<<(std::wostream& out, const PropertyType& propertyType)
{
	switch(propertyType) {
		case PropertyType::Identifier: return out << L"Identifier";
		case PropertyType::Source: return out << L"Source";
		case PropertyType::Constant: return out << L"Constant";
		case PropertyType::Closure: return out << L"Closure";
		case PropertyType::Order: return out << L"Order";
		case PropertyType::Stack: return out << L"Stack";
		case PropertyType::ReturnStack: return out << L"ReturnStack";
		case PropertyType::Native: return out << L"Native";
		case PropertyType::StackVM: return out << L"StackVM";
		case PropertyType::Escape: return out << L"Escape";
		case PropertyType::Proposition: return out << L"Proposition";
		default: throw std::domain_error("Illegal enum value.");
	}
}
