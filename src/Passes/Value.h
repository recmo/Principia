#pragma once
#include "fixups.h"

class Closure;
class Value;
typedef vector<Value>(*BuiltinFunction)(const vector<Value>&);

class Value
{
public:
	Value() : kind(None) {} // STL needs constructor without args
	Value(const Closure* value) : kind(Function), _closure(value) {}
	Value(const sint64 value) : kind(Integer), _integer(value) {}
	Value(const double value) : kind(Real), _real(value) {}
	Value(const BuiltinFunction value) : kind(Builtin), _builtin(value) {}
	Value(const string& value) : kind(String), _string(new string(value)) {}
	
	bool operator==(const Value& other) const;
	const Closure* closure() const { assert(kind == Function); return _closure; }
	sint64 integer() const { assert(kind == Integer); return _integer; }
	double real() const { assert(kind == Real); return _real; }
	BuiltinFunction builtin() const { assert(kind == Builtin); return _builtin; }
	string stringValue() const { assert(kind == String); return *_string; }
	
	enum Kind {
		None,
		Function,
		Integer,
		Real,
		Builtin,
		String
	};
	
	Kind kind;
	
private:
	union {
		const Closure* _closure;
		sint64 _integer;
		double _real;
		BuiltinFunction _builtin;
		string* _string;
	};
};

std::wostream& operator<<(std::wostream& out, const Value& value);

inline std::ostream& operator<<(std::ostream& out, const Value& value)
{
	return out << encodeLocal(toString(value));
}
