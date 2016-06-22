#include "pretty_print.h"
#include <Unicode/string.h>
#include <Utilities/string.h>
#include <UnitTest++.h>

using std::vector;
using std::set;

SUITE(Utilities) {

TEST(NullPointers)
{
	CHECK_EQUAL(string(L"null"), toString(std::unique_ptr<int>()));
	CHECK_EQUAL(string(L"null"), toString(std::shared_ptr<int>()));
	CHECK_EQUAL(string(L"null"), toString(std::weak_ptr<int>()));
}

TEST(EmptyIntVector)
{
	const vector<int> test{};
	const string result = toString(test);
	CHECK_EQUAL(string(L"[]"), result);
}

TEST(IntVector123)
{
	const vector<int> test{1,2,3};
	const string result = toString(test);
	CHECK_EQUAL(string(L"[1, 2, 3]"), result);
}

TEST(EmptyStringVector)
{
	const vector<string> test{};
	const string result = toString(test);
	CHECK_EQUAL(string(L"[]"), result);
}

TEST(StringVectorABC)
{
	const vector<string> test{L"A", L"B", L"C"};
	const string result = toString(test);
	CHECK_EQUAL(string(L"[A, B, C]"), result);
}

TEST(EmptyIntSet)
{
	const set<int> test;
	const string result = toString(test);
	CHECK_EQUAL(string(L"{}"), result);
}

TEST(EmptyIntSet123)
{
	const set<int> test{2, 3, 1};
	const string result = toString(test);
	CHECK_EQUAL(string(L"{1, 2, 3}"), result);
}

} // SUITE
