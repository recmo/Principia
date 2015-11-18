#include "Builtin.h"
typedef Value::Values Values;

Value Builtin::lookup(const string& name)
{
	if(name == L"if" ) return Builtin::if_;
	if(name == L"add") return Builtin::add;
	if(name == L"sub") return Builtin::sub;
	if(name == L"mul") return Builtin::mul;
	if(name == L"div") return Builtin::div;
	return Value();
}

const Value Builtin::if_{[](const Values& arg) -> Values {
	assert(arg.size() == 3);
	Value result = (arg[0].integer()) ? arg[1] : arg[2]; 
	return Values{result};
}};

const Value Builtin::add{[](const Values& arg) -> Values {
	assert(arg.size() == 2);
	Value result{arg[0].integer() + arg[1].integer()};
	return Values{result};
}};

const Value Builtin::sub{[](const Values& arg) -> Values {
	assert(arg.size() == 2);
	Value result{arg[0].integer() - arg[1].integer()};
	return Values{result};
}};

const Value Builtin::mul{[](const Values& arg) -> Values {
	assert(arg.size() == 2);
	Value result{arg[0].integer() * arg[1].integer()};
	return Values{result};
}};

const Value Builtin::div{[](const Values& arg) -> Values {
	assert(arg.size() == 2);
	Value result1{arg[0].integer() / arg[1].integer()};
	Value result2{arg[0].integer() % arg[1].integer()};
	return Values{result1, result2};
}};
