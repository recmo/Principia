#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <Parser/Compile.h>
namespace Interpreter {
using Compile::Program;
using Compile::Function;
using Compile::Symbol;

enum Kind {
	Import,
	Constant,
	Closure
};

struct Value {
	Kind kind;
	std::wstring string;
	uint closure;
	std::vector<Value> values;
};

typedef std::vector<Value> Values;


void run(const Program& p, const std::wstring& function);

void run(const Program& p, const Function& function);

void run(const Program& p, const Function& function, const Values& closure,
	const Values& arguments);

void run(const Program& p, const Value& function, const Values& arguments);

void run_builtin(const Program& p, const std::wstring& function,
	const Values& arguments);

};

std::wostream& operator<<(std::wostream& out, const Interpreter::Value& value);
