#include "Value.h"
#include <UnitTest++/UnitTest++.h>

SUITE(Value) {

Value::Values func(const Value::Values& args)
{
	return args;
}

TEST(Construct)
{
	Value v1{};
	Value v2{1L};
	Value v3{1.0};
	Value v4{func};
	Value v5{L"Test"};
	CHECK_EQUAL(Value::None, v1.type());
	CHECK_EQUAL(Value::Integer, v2.type());
	CHECK_EQUAL(Value::Real, v3.type());
	CHECK_EQUAL(Value::ExtFunc, v4.type());
	CHECK_EQUAL(Value::String, v5.type());
	CHECK_EQUAL(1L, v2.integer());
	CHECK_EQUAL(1.0, v3.real());
	CHECK_EQUAL(L"Test", v5.string());
	CHECK_THROW(v1.closure(), assertion_failed);
}

TEST(Assign)
{
	Value v;
	v = Value{};
	v = Value{1L};
	v = Value{1.0};
	v = Value{func};
	v = Value{L"Test"};
}

} // SUITE
