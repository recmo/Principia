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
	enum Kind {
		None,
		Closure,
		Integer,
		Real,
		ExtFunc,
		String
	};
	Value() = default;
	explicit Value(::Closure* value): _kind(Closure), _closure(value) { }
	explicit Value(sint64 value): _kind(Integer), _integer(value) { }
	explicit Value(double value): _kind(Real), _real(value) { }
	explicit Value(Func value): _kind(ExtFunc), _func(value) { }
	explicit Value(::string value): _kind(String), _string(value) { }
	bool operator==(const Value& other) const;
	
	Kind kind() const { return _kind; }
	const ::Closure* closure() const { assert(_kind == Closure); return _closure; }
	sint64 integer() const { assert(_kind == Integer); return _integer; }
	double real() const { assert(_kind == Real); return _real; }
	Func func() const { assert(_kind == ExtFunc); return _func; }
	::string string() const { assert(_kind == String); return _string; }
	
private:
	Kind _kind = None;
	::Closure* _closure = nullptr;
	sint64 _integer = 0LL;
	double _real = 0.0;
	Func _func{};
	::string _string{};
};

std::wostream& operator<<(std::wostream& out, Value::Kind value);

std::wostream& operator<<(std::wostream& out, const Value& value);
