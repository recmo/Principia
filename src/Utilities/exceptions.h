#pragma once
#include <stdexcept>

class index_out_of_range: public std::out_of_range
{
public:
	index_out_of_range()
	: out_of_range("The index is out of range.") { }
};

class unimplemented: public std::logic_error
{
public:
	unimplemented()
	: logic_error("The behaviour is unimplemented.") { }
};

class invalid_enum: public std::domain_error
{
public:
	invalid_enum()
	: domain_error("The enum value is not supported.") { }
};
