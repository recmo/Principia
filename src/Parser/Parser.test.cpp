#include <Parser/Parser.h>
#include <UnitTest++/UnitTest++.h>
#include <Utilities/testExamples.h>
#include <stdexcept>

SUITE(Parser) {

TEST(ParseLiterals)
{
	Parser().parse(std::string{ "asd ≔ (↦ 3)"});
	Parser().parse(string{L"asd ≔ (↦ 3)"});
}

void testIdempotent(ParseTree* tree)
{
	string source = toString(*tree);
	Parser p;
	p.parse(source);
	ParseTree* t = p.tree();
	string source2 = toString(*t);
	CHECK(source == source2);
}

void testExample(const string& filename)
{
	Parser p;
	p.parseFile(filename);
	ParseTree* t = p.tree();
	CHECK(t->validate());
	testIdempotent(t);
}

TestExamples;

TEST(DoesNotExits)
{
	CHECK_THROW(Parser().parseFile(L"/does-not-exist"), std::runtime_error);
}

TEST(ParseFactorial)
{
	const string factorial{
		L"fact n ↦ r\n"
		"\tif_zero d ↦ 1\n"
		"\totherwise d ↦ or\n"
		"\t\tor ≔ mul n (≔ fact (≔ sub n 1))\n"
		"\tfr ≔ if n otherwise if_zero\n"
		"\tr ≔ fr n\n"
	};
	Parser().parse(std::string{"asd ≔ (↦ 3)"});
	Parser().parse(string{L"asd ≔ (↦ 3)"});
	Parser().parse(factorial);
}

} // SUITE
