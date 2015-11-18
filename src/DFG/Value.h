#pragma once
#include <Utilities/inttypes.h>
#include <Utilities/assert.h>
#include <Unicode/string.h>
#include <functional>
#include <memory>
#include <vector>
class Closure;
class Value;

class Value
{
public:
	typedef std::vector<Value> Values;
	typedef std::function<Values(const Values&)> Func;
	enum Type {
		None,
		Closure,
		Integer,
		Real,
		ExtFunc,
		String
	};
	Value() = default;
	explicit Value(::Closure* value): _type(Closure), _closure(value) { }
	explicit Value(sint64 value): _type(Integer), _integer(value) { }
	explicit Value(double value): _type(Real), _real(value) { }
	explicit Value(Func value): _type(ExtFunc), _func(value) { }
	explicit Value(::string value): _type(String), _string(value) { }
	bool operator==(const Value& other) const;
	
	Type type() const { return _type; }
	const ::Closure* closure() const { assert(_type == Closure); return _closure; }
	sint64 integer() const { assert(_type == Integer); return _integer; }
	double real() const { assert(_type == Real); return _real; }
	Func func() const { assert(_type == ExtFunc); return _func; }
	::string string() const { assert(_type == String); return _string; }
	
private:
	Type _type = None;
	::Closure* _closure = nullptr;
	sint64 _integer = 0LL;
	double _real = 0.0;
	Func _func{};
	::string _string{};
};

std::wostream& operator<<(std::wostream& out, Value::Type value);

std::wostream& operator<<(std::wostream& out, const Value& value);

std::ostream& operator<<(std::ostream& out, const Value& value);
