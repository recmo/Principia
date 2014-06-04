#include "fixups.h"
#include "Builtins.h"
#include "Closure.h"
#include <DFG/Node.h>

BuiltinsStatic builtins;

vector<Value> builtin_none(const vector<Value>& arg)
{
	vector<Value> ret;
	return ret;
}

vector<Value> builtin_if(const vector<Value>& arg)
{
	assert(arg.size() == 3);
	vector<Value> ret;
	ret.push_back((arg[0].integer()) ? arg[1] : arg[2]);
	return ret;
}

// Integer arithmetic

vector<Value> builtin_add(const vector<Value>& arg)
{
	assert(arg.size() == 2);
	vector<Value> ret;
	ret.push_back(arg[0].integer() + arg[1].integer());
	return ret;
}

vector<Value> builtin_sub(const vector<Value>& arg)
{
	assert(arg.size() == 2);
	vector<Value> ret;
	ret.push_back(arg[0].integer() - arg[1].integer());
	return ret;
}

vector<Value> builtin_mul(const vector<Value>& arg)
{
	assert(arg.size() == 2);
	vector<Value> ret;
	ret.push_back(arg[0].integer() * arg[1].integer());
	return ret;
}

vector<Value> builtin_div(const vector<Value>& arg)
{
	assert(arg.size() == 2);
	vector<Value> ret;
	ret.push_back(arg[0].integer() / arg[1].integer());
	ret.push_back(arg[0].integer() % arg[1].integer());
	return ret;
}

// Integer float conversion

vector<Value> builtin_real(const vector<Value>& arg)
{
	assert(arg.size() == 1);
	vector<Value> ret;
	ret.push_back(static_cast<double>(arg[0].integer()));
	return ret;
}

vector<Value> builtin_int(const vector<Value>& arg)
{
	assert(arg.size() == 1);
	vector<Value> ret;
	ret.push_back(static_cast<sint64>(arg[0].real()));
	return ret;
}

// Floating point arithmetic

vector<Value> builtin_addr(const vector<Value>& arg)
{
	assert(arg.size() == 2);
	vector<Value> ret;
	double result = arg[0].real() + arg[1].real();
	ret.push_back(result);
	return ret;
}

vector<Value> builtin_subr(const vector<Value>& arg)
{
	assert(arg.size() == 2);
	vector<Value> ret;
	double result = arg[0].real() - arg[1].real();
	ret.push_back(result);
	return ret;
}

vector<Value> builtin_mulr(const vector<Value>& arg)
{
	assert(arg.size() == 2);
	vector<Value> ret;
	double result = arg[0].real() * arg[1].real();
	ret.push_back(result);
	return ret;
}

vector<Value> builtin_divr(const vector<Value>& arg)
{
	assert(arg.size() == 2);
	vector<Value> ret;
	double result = arg[0].real() / arg[1].real();
	ret.push_back(result);
	return ret;
}

//
// String processing
//

vector<Value> length(const vector<Value>& arg)
{
	assert(arg.size() == 1);
	string s = arg[0].stringValue();
	sint64 result = s.size();
	vector<Value> ret;
	ret.push_back(Value(result));
	return ret;
}

vector<Value> concat(const vector<Value>& arg)
{
	// concat left right ↦ r
	// r = left ∷ right
	assert(arg.size() == 2);
	string left = arg[0].stringValue();
	string right = arg[1].stringValue();
	string result = left + right;
	vector<Value> ret;
	ret.push_back(Value(result));
	return ret;
}

vector<Value> substring(const vector<Value>& arg)
{
	// substring s start end ↦ r
	// r = s[start…end]
	assert(arg.size() == 3);
	string s = arg[0].stringValue();
	sint64 start = arg[1].integer();
	sint64 end = arg[2].integer();
	assert(start >= 0);
	assert(start <= end);
	assert(end < s.size());
	string result = s.substr(start, end - start);
	vector<Value> ret;
	ret.push_back(Value(result));
	return ret;
}

vector<Value> equals(const vector<Value>& arg)
{
	// equals a b ↦ r
	// r = true if the strings are equal, false otherwise
	assert(arg.size() == 2);
	string left = arg[0].stringValue();
	string right = arg[1].stringValue();
	sint64 result = (left == right) ? 1 : 0;
	vector<Value> ret;
	ret.push_back(Value(result));
	return ret;
}

//
// Unicode functions
//
// These allow implementation of an ordering of strings
// by reinterpretting them as arrays of codepoints
// and sorting those arrays lexicographically

vector<Value> codepoint(const vector<Value>& arg)
{
	// codepoint c ↦ r
	// r = unicode codepoint of char c
	assert(arg.size() == 1);
	string c = arg[0].stringValue();
	assert(c.size() == 1);
	wchar_t ct = c[0];
	sint64 cp = ct;
	vector<Value> ret;
	ret.push_back(Value(cp));
	return ret;
}

vector<Value> character(const vector<Value>& arg)
{
	// character c ↦ r
	// r = unicode codepoint of char c
	assert(arg.size() == 1);
	sint64 cp = arg[0].integer();
	assert(cp >= 0);
	assert(cp <= 0x10FFFF);
	vector<Value> ret;
	ret.push_back(string(1, cp));
	return ret;
}

//
//  I O   ¡¡ non pure !!
//

vector<Value> write(const vector<Value>& arg)
{
	throw "Not implemented";
	return vector<Value>();
}

vector<Value> read(const vector<Value>& arg)
{
	throw "Not implemented";
	return vector<Value>();
}


//
// Reflection
//

vector<Value> nodeType(const vector<Value>& arg)
{
	// isNodeType closure ↦ node_type
	// Distinguish between call and closure nodes
	throw "Not implemented";
	return vector<Value>();
}

vector<Value> intensionalEquivalent(const vector<Value>& arg)
{
	throw "Not implemented";
	return vector<Value>();
}

vector<Value> call(const vector<Value>& arg)
{
	// call num_inputs num_outputs ↦ closure
	throw "Not implemented";
	return vector<Value>();
}

vector<Value> closure(const vector<Value>& arg)
{
	// closure num_inputs num_outputs ↦ closure
	throw "Not implemented";
	return vector<Value>();
}

vector<Value> arity(const vector<Value>& arg)
{
	assert(arg.size() == 1);
	Value closure = arg[0];
	vector<Value> ret;
	if(closure.kind == Value::Builtin) {
		// TODO
	} else {
		sint64 num_inputs = closure.closure()->node()->inArity();
		sint64 num_outputs = closure.closure()->node()->outArity() - 1;
		ret.push_back(Value(num_inputs));
		ret.push_back(Value(num_outputs));
	}
	return ret;
}

vector<Value> input(const vector<Value>& arg)
{
	// input closure index ↦ closure
	throw "Not implemented";
	return vector<Value>();
}

vector<Value> output(const vector<Value>& arg)
{
	// output closure index ↦ closure
	throw "Not implemented";
	return vector<Value>();
}

vector<Value> strconcat(const vector<Value>& arg)
{
	// concat low_closure high_closure ↦ closure
	throw "Not implemented";
	return vector<Value>();
}

vector<Value> compose(const vector<Value>& arg)
{
	// make_closure in_closure out_closure ↦ closure
	throw "Not implemented";
	return vector<Value>();
}

BuiltinsStatic::BuiltinsStatic()
{
	insert(value_type(L"none", &builtin_none));
	insert(value_type(L"if", &builtin_if));
	insert(value_type(L"add", &builtin_add));
	insert(value_type(L"mul", &builtin_mul));
	insert(value_type(L"sub", &builtin_sub));
	insert(value_type(L"div", &builtin_div));
	insert(value_type(L"real", &builtin_real));
	insert(value_type(L"int", &builtin_int));
	insert(value_type(L"addr", &builtin_addr));
	insert(value_type(L"mulr", &builtin_mulr));
	insert(value_type(L"subr", &builtin_subr));
	insert(value_type(L"divr", &builtin_divr));
	insert(value_type(L"≡", &intensionalEquivalent));
}
