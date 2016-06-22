#include "convert.h"
#include <Unicode/string.h>
#include <UnitTest++.h>

SUITE(Unicode) {

TEST(EncodeDecodeLocal)
{
	const std::wstring test(L"Test string");
	const std::string encoded = encodeLocal(test);
	CHECK_EQUAL(encoded, "Test string");
	const std::wstring decoded = decodeLocal(encoded);
	CHECK_EQUAL(test, decoded);
}

TEST(EncodeDecodeUtf8)
{
	const std::wstring test(L"Test string");
	const std::string encoded = encodeUtf8(test);
	CHECK_EQUAL(encoded, "Test string");
	const std::wstring decoded = decodeUtf8(encoded);
	CHECK_EQUAL(test, decoded);
}

TEST(DecodeEncodeUtf8)
{
	const std::string test("Test string");
	const std::wstring decoded = decodeUtf8(test);
	CHECK_EQUAL(decoded, L"Test string");
	const std::string encoded = encodeUtf8(decoded);
	CHECK_EQUAL(test, encoded);
}

TEST(ComplexEncodeDecodeLocal)
{
	const std::wstring test(L"Test ∃ ∀ ∫ x×y⁷·π dx");
	const std::string encoded = encodeLocal(test);
	const std::wstring decoded = decodeLocal(encoded);
	CHECK_EQUAL(test, decoded);
}

TEST(ComplexEncodeDecodeUtf8)
{
	const std::wstring test(L"Test ∃ ∀ ∫ x×y⁷·π dx");
	const std::string encoded = encodeUtf8(test);
	const std::wstring decoded = decodeUtf8(encoded);
	CHECK_EQUAL(test, decoded);
}

} // SUITE
