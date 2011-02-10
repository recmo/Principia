#include "fixups.h"
#include "Builtins.h"

BuiltinsStatic builtins;

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

BuiltinsStatic::BuiltinsStatic()
{
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
}
