#include "inttypes.h"
#include <UnitTest++/UnitTest++.h>
#include <limits>

SUITE(Utilities) {

TEST(IntTypesSizes)
{
	CHECK_EQUAL(1, sizeof(uint8));
	CHECK_EQUAL(2, sizeof(uint16));
	CHECK_EQUAL(4, sizeof(uint32));
	CHECK_EQUAL(8, sizeof(uint64));
	CHECK_EQUAL(1, sizeof(sint8));
	CHECK_EQUAL(2, sizeof(sint16));
	CHECK_EQUAL(4, sizeof(sint32));
	CHECK_EQUAL(8, sizeof(sint64));
}

TEST(IntTypesRanges)
{
	CHECK_EQUAL(0ULL, std::numeric_limits<uint8>().min());
	CHECK_EQUAL(0ULL, std::numeric_limits<uint16>().min());
	CHECK_EQUAL(0ULL, std::numeric_limits<uint32>().min());
	CHECK_EQUAL(0ULL, std::numeric_limits<uint64>().min());
	CHECK_EQUAL(255ULL, std::numeric_limits<uint8>().max());
	CHECK_EQUAL(65535ULL, std::numeric_limits<uint16>().max());
	CHECK_EQUAL(4294967295ULL, std::numeric_limits<uint32>().max());
	CHECK_EQUAL(18446744073709551615ULL, std::numeric_limits<uint64>().max());
	CHECK_EQUAL(-128LL, std::numeric_limits<sint8>().min());
	CHECK_EQUAL(-32768LL, std::numeric_limits<sint16>().min());
	CHECK_EQUAL(-2147483648LL, std::numeric_limits<sint32>().min());
	CHECK_EQUAL(-9223372036854775808LL, std::numeric_limits<sint64>().min());
	CHECK_EQUAL(127ULL, std::numeric_limits<sint8>().max());
	CHECK_EQUAL(32767ULL, std::numeric_limits<sint16>().max());
	CHECK_EQUAL(2147483647ULL, std::numeric_limits<sint32>().max());
	CHECK_EQUAL(9223372036854775807ULL, std::numeric_limits<sint64>().max());
}

} // SUITE
