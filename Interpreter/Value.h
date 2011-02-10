#pragma once
#include "fixups.h"

class Closure;

class Value
{
public:
	Value() : kind(None) {} // STL needs constructor without args
	Value(const Closure* value) : kind(Function), _function(value) {}
	Value(const sint64 value) : kind(Integer), _integer(value) {}
	Value(const double value) : kind(Real), _real(value) {}
	
	const Closure* function() const { assert(kind == Function); return _function; }
	sint64 integer() const { assert(kind == Integer); return _integer; }
	double real() const { assert(kind == Real); return _real; }
	
	enum Kind
	{
		None,
		Function,
		Integer,
		Real
	};
	
	Kind kind;
	
private:
	union
	{
		const Closure* _function;
		sint64 _integer;
		double _real;
	};
	void delete_function();
};

std::wostream& operator<<(std::wostream& out, const Value& value);


