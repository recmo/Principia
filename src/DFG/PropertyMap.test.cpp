#include "PropertyMap.h"
#include <Parser/ConstantProperty.h>
#include <UnitTest++/UnitTest++.h>

SUITE(PropertyMap) {

TEST(Construct)
{
	PropertyMap p;
}

TEST(DefaultHasNoProperties)
{
	PropertyMap p;
	CHECK_EQUAL(p.has(PropertyType::Constant), false);
	CHECK_EQUAL(p.has<ConstantProperty>(), false);
}

TEST(SetConstant)
{
	PropertyMap p;
	ConstantProperty cp(Value(L"Test value"));
	p.set(cp);
	CHECK_EQUAL(p.has(PropertyType::Constant), true);
	CHECK_EQUAL(p.has<ConstantProperty>(), true);
	CHECK_EQUAL(p.get<ConstantProperty>().value(), cp.value());
}

} // SUITE
