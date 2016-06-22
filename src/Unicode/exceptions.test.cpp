#include "exceptions.h"
#include <Unicode/convert.h>
#include <UnitTest++.h>

SUITE(Unicode) {

TEST(Construct)
{
	const runtime_error e(L"Test exception");
	CHECK_EQUAL(e.what(), "Test exception");
}

TEST(ComplexConstruct)
{
	const runtime_error e(L"Test ∃ ∀ ∫ x×y⁷·π dx");
	CHECK_EQUAL(e.what(), encodeLocal(L"Test ∃ ∀ ∫ x×y⁷·π dx"));
}

TEST(ThrowExceptions)
{
	const runtime_error e(L"Test exception");
	CHECK_THROW(throw e, std::runtime_error);
}

TEST(ThrowComplexExceptions)
{
	const runtime_error e(L"Test ∃ ∀ ∫ x×y⁷·π dx");
	CHECK_THROW(throw e, std::runtime_error);
}

} // SUITE
