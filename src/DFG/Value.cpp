#include "Value.h"
#include <Utilities/exceptions.h>
#include <Utilities/string.h>
#include <Unicode/convert.h>

bool Value::operator==(const Value& other) const
{
	if(_kind != other._kind)
		return false;
	switch(_kind) {
		case Value::None: return true;
		case Value::Closure: throw unimplemented{};
		case Value::Integer: return integer() == other.integer();
		case Value::Real: return real() == other.real();
		case Value::ExtFunc: throw unimplemented{};
		case Value::String: return string() == other.string();
		default: throw invalid_enum{};
	}
}

std::wostream& operator<<(std::wostream& out, Value::Kind value)
{
	switch(value) {
		case Value::None: return out << L"None";
		case Value::Closure: return out << L"Closure";
		case Value::Integer: return out << L"Integer";
		case Value::Real: return out << L"Real";
		case Value::ExtFunc: return out << L"ExtFunc";
		case Value::String: return out << L"String";
		default: throw invalid_enum{};
	}
}

std::wostream& operator<<(std::wostream& out, const Value& value)
{
	switch(value.kind()) {
		case Value::None:
			out << L"none";
			break;
		case Value::Closure:
			out << L"closure";
			// out << value.closure()->node();// << value.closure()->context();
			break;
		case Value::Integer:
			out << value.integer();
			break;
		case Value::Real:
			out.precision(5);
			out << std::fixed << value.real();
			break;
		case Value::ExtFunc: {
			out << L"function";
			break;
		}
		case Value::String:
			out << L"“" << value.string() << L"”";
			break;
		default: throw invalid_enum{};
	}
	return out;
}

std::ostream& operator<<(std::ostream& out, const Value& value)
{
	return out << encodeLocal(toString(value));
}
