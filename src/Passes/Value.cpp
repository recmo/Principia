#include "Value.h"
#include "DFG/Node.h"
#include "Passes/Builtins.h"
#include "Closure.h"

bool Value::operator==(const Value& other) const
{
	if(kind != other.kind)
		return false;
	switch(kind) {
		case Value::None: return true;
		case Value::Function: throw runtime_error(L"Unimplemented");
		case Value::Integer: return integer() == other.integer();
		case Value::Real: return real() == other.real();
		case Value::Builtin: throw runtime_error(L"Unimplemented");
		case Value::String: return stringValue() == other.stringValue();
		default: throw runtime_error(L"Unimplemented");
	}
}

std::wostream& operator<<(std::wostream& out, const Value& value)
{
	switch(value.kind) {
		case Value::None:
			out << L"none";
			break;
		case Value::Function:
			out << value.closure()->node();// << value.closure()->context();
			break;
		case Value::Integer:
			out << value.integer();
			break;
		case Value::Real:
			out.precision(5);
			out << std::fixed << value.real();
			break;
		case Value::Builtin: {
			string name;
			if(tryGetKey<string, BuiltinFunction>(builtins, value.builtin(), name))
				out << L"builtin_" << name;
			else
				out << L"builtin";
			break;
		}
		case Value::String:
			out << L"“" << value.stringValue() << L"”";
			break;
		default:
			wcerr << endl << L"Unimplemented type " << value.kind << endl;
			throw "Unimplemented type";
	}
	return out;
}

