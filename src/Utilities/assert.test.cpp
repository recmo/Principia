#include "assert.h"
#include <Unicode/string.h>
#include <Utilities/string.h>
#include <UnitTest++.h>

SUITE(Utilities) {

TEST(AssertTrue)
{
	assert(true);
}

TEST(AssertFalse)
{
	CHECK_THROW(assert(false), assertion_failed);
}

TEST(AssertEvalsOnce)
{
	int i = 0;
	assert(++i || true);
	assert(++i || true);
	CHECK_EQUAL(2, i);
}

} // SUITE
