#include "Builtin.h"
typedef Value::Values Values;;

const Value Builtin::if_{[](const Values& arg) -> Values {
	assert(arg.size() == 3);
	Value result = (arg[0].integer()) ? arg[1] : arg[2]; 
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