#pragma once
#include <DFG/Value.h>
#include <Unicode/string.h>

namespace Builtin {
	Value lookup(const string& name);
	extern const Value if_;
	extern const Value add;
	extern const Value sub;
	extern const Value mul;
	extern const Value div;
};
