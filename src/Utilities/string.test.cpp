#include "pretty_print.h"
#include <Unicode/string.h>
#include <Utilities/string.h>
#include <UnitTest++/UnitTest++.h>

using std::vector;
using std::set;

SUITE(Utilities) {

TEST(TokenizeEmpty)
{
	const vector<string> tokens = tokenize(L"");
	CHECK_EQUAL(0, tokens.size());
	CHECK_ARRAY_EQUAL(vector<string>{}, tokens, 0);
}

TEST(TokenizeSpace)
{
	const vector<string> tokens = tokenize(L" ");
	CHECK_EQUAL(0, tokens.size());
	CHECK_ARRAY_EQUAL(vector<string>{}, tokens, 0);
}

TEST(TokenizeWhitespace)
{
	const vector<string> tokens = tokenize(L"  \t \r\n\r\n  ");
	CHECK_EQUAL(0, tokens.size());
	CHECK_ARRAY_EQUAL(vector<string>{}, tokens, 0);
}

TEST(TokenizeFooBar)
{
	const vector<string> tokens = tokenize(L"  \t foo \r\nbar\r\n  ");
	CHECK_EQUAL(2, tokens.size());
	const vector<string> expected{L"foo", L"bar"};
	CHECK_ARRAY_EQUAL(expected, tokens, 2);
}

TEST(EmptyStartsWith)
{
	const string empty;
	const string nonempty{L"Non empty"};
	CHECK_EQUAL(true, startsWith(empty, empty));
	CHECK_EQUAL(false, startsWith(empty, nonempty));
	CHECK_EQUAL(true, startsWith(nonempty, empty));
	CHECK_EQUAL(true, startsWith(nonempty, nonempty));
}

TEST(StartsWith)
{
	CHECK_EQUAL(true, startsWith(L"FooBar", L"F"));
	CHECK_EQUAL(true, startsWith(L"FooBar", L"Foo"));
	CHECK_EQUAL(false, startsWith(L"FooBar", L"Bar"));
}

TEST(ToStringTest)
{
	CHECK_EQUAL(string{L"FooBar"}, toString(L"FooBar"));
	CHECK_EQUAL(string{L"FooBar"}, toString("FooBar"));
	CHECK_EQUAL(string{L"123"}, toString(123));
	CHECK_EQUAL(string{L"1.23"}, toString(1.23));
	CHECK_EQUAL(string{L"{1, 2, 3}"}, toString(set<int>{3,1,2}));
}

TEST(ParseTest)
{
	CHECK_EQUAL(string{L"FooBar"}, parse<string>(L"FooBar"));
	CHECK_EQUAL(string{L"FooBar"}, parse<string>(L"   FooBar"));
	CHECK_EQUAL(123, parse<int>(L"123"));
	CHECK_CLOSE(1.23, parse<float>(L"1.23"), 0.001);
	CHECK_CLOSE(1.23, parse<double>(L"1.23"), 0.001);
	CHECK_THROW(parse<int>(L"abc"), parse_error);
	CHECK_THROW(parse<int>(L"1 2 3"), parse_error);
	CHECK_THROW(parse<string>(L"A B C"), parse_error);
}

}
