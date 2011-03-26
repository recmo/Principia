#include "BindingType.h"

string BindingType::toString() const
{
	switch(value)
	{
		case BindingType::External: return L"Undefined";
		case BindingType::Return: return L"Return";
		case BindingType::Closure: return L"Function";
		case BindingType::Argument: return L"Argument";
		default: return L"Illegal value";
	}
}

