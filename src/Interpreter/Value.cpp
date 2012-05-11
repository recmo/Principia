#include "Value.h"
#include "DFG/Node.h"
#include "Interpreter/Builtins.h"
#include "Closure.h"

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
