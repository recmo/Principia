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

// Integer arithmetic

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

vector<uint64> builtin_div(const vector<uint64>& arg)
{
	assert(arg.size() == 2);
	vector<uint64> ret;
	ret.push_back(arg[0] / arg[1]);
	ret.push_back(arg[0] % arg[1]);
	return ret;
}

// Integer float conversion

uint64 fasi(const double d)
{
	const double* dp = &d;
	const uint64* ip = reinterpret_cast<const uint64*>(dp);
	const uint64 i = *ip;
	return i;
}

double iasf(const uint64 i)
{
	const uint64* ip = &i;
	const double* dp = reinterpret_cast<const double*>(ip);
	const double d = *dp;
	return d;
}

vector<uint64> builtin_itof(const vector<uint64>& arg)
{
	assert(arg.size() == 1);
	vector<uint64> ret;
	ret.push_back(fasi(static_cast<double>(arg[0])));
	return ret;
}

vector<uint64> builtin_ftoi(const vector<uint64>& arg)
{
	assert(arg.size() == 1);
	vector<uint64> ret;
	ret.push_back(static_cast<uint64>(iasf(arg[0])));
	return ret;
}

// Floating point arithmetic

vector<uint64> builtin_addf(const vector<uint64>& arg)
{
	assert(arg.size() == 2);
	vector<uint64> ret;
	
	wcerr << L"addf(" << iasf(arg[0]) << L", " << iasf(arg[1]) << L")" << endl; 
	
	double result = iasf(arg[0]) + iasf(arg[1]);
	ret.push_back(fasi(result));
	return ret;
}

vector<uint64> builtin_subf(const vector<uint64>& arg)
{
	assert(arg.size() == 2);
	vector<uint64> ret;
	wcerr << L"subf(" << iasf(arg[0]) << L", " << iasf(arg[1]) << L")" << endl; 
	double result = iasf(arg[0]) - iasf(arg[1]);
	ret.push_back(fasi(result));
	return ret;
}

vector<uint64> builtin_mulf(const vector<uint64>& arg)
{
	assert(arg.size() == 2);
	vector<uint64> ret;
	wcerr << L"mulf(" << iasf(arg[0]) << L", " << iasf(arg[1]) << L")" << endl; 
	double result = iasf(arg[0]) * iasf(arg[1]);
	ret.push_back(fasi(result));
	return ret;
}

vector<uint64> builtin_divf(const vector<uint64>& arg)
{
	assert(arg.size() == 2);
	vector<uint64> ret;
	wcerr << L"divf(" << iasf(arg[0]) << L", " << iasf(arg[1]) << L")" << endl; 
	double result = iasf(arg[0]) / iasf(arg[1]);
	ret.push_back(fasi(result));
	return ret;
}

BuiltinsStatic::BuiltinsStatic()
{
	insert(value_type(L"if", &builtin_if));
	insert(value_type(L"add", &builtin_add));
	insert(value_type(L"mul", &builtin_mul));
	insert(value_type(L"sub", &builtin_sub));
	insert(value_type(L"div", &builtin_div));
	insert(value_type(L"itof", &builtin_itof));
	insert(value_type(L"ftoi", &builtin_ftoi));
	insert(value_type(L"addf", &builtin_addf));
	insert(value_type(L"mulf", &builtin_mulf));
	insert(value_type(L"subf", &builtin_subf));
	insert(value_type(L"divf", &builtin_divf));
}
