#include "fixups.h"
#include "Builtins.h"

BuiltinsStatic builtins;

vector<uint64> builtin_if(const vector<uint64>& arg)
{
	assert(arg.size() == 3);
	vector<uint64> ret;
	ret.push_back((arg[0]) ? arg[1] : arg[2]);
	return ret;
}

vector<uint64> builtin_add(const vector<uint64>& arg)
{
	assert(arg.size() == 2);
	vector<uint64> ret;
	ret.push_back(arg[0] + arg[1]);
	return ret;
}

vector<uint64> builtin_sub(const vector<uint64>& arg)
{
	assert(arg.size() == 2);
	vector<uint64> ret;
	ret.push_back(arg[0] - arg[1]);
	return ret;
}

vector<uint64> builtin_mul(const vector<uint64>& arg)
{
	assert(arg.size() == 2);
	vector<uint64> ret;
	ret.push_back(arg[0] * arg[1]);
	return ret;
}

BuiltinsStatic::BuiltinsStatic()
{
	insert(value_type(L"if", &builtin_if));
	insert(value_type(L"add", &builtin_add));
	insert(value_type(L"mul", &builtin_mul));
	insert(value_type(L"sub", &builtin_sub));
}
