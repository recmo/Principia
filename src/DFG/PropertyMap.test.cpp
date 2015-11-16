#include "PropertyMap.h"
#include <UnitTest++/UnitTest++.h>

SUITE(PropertyMap) {

class TestProperty: public PropertyMap::Property
{
public:
	TestProperty() { }
	TestProperty(int v): value(v) { }
	
	bool operator==(const TestProperty& other) const {
		return value == other.value;
	}
	
	int value = 0;
	
	friend std::wostream& operator<<(std::wostream& out, const TestProperty& property);
	virtual void print(std::wostream& out) const override { out << *this; }
};

inline std::wostream& operator<<(std::wostream& out, const TestProperty& property)
{
	return out << L"Test property " << property.value;
}

inline std::ostream& operator<<(std::ostream& out, const TestProperty& property)
{
	return out << "Test property " << property.value;
}

TEST(Construct)
{
	PropertyMap p;
	TestProperty tp;
}

TEST(SetHasGetChangeGet)
{
	PropertyMap p;
	const PropertyMap& constp = p;
	
	// Check the unset state
	CHECK_EQUAL(false, p.has<TestProperty>());
	CHECK_EQUAL(false, constp.has<TestProperty>());
	CHECK_THROW(p.get<TestProperty>(), not_found);
	CHECK_THROW(constp.get<TestProperty>(), not_found);
	
	// Check the set state
	p.set(TestProperty{4});
	CHECK_EQUAL(true, p.has<TestProperty>());
	CHECK_EQUAL(true, constp.has<TestProperty>());
	CHECK_EQUAL(4, p.get<TestProperty>().value);
	CHECK_EQUAL(4, constp.get<TestProperty>().value);
	
	// Change the value in place
	p.get<TestProperty>().value++;
	CHECK_EQUAL(5, p.get<TestProperty>().value);
	CHECK_EQUAL(5, constp.get<TestProperty>().value);
	
	// Override with new value
	p.set(TestProperty{3});
	CHECK_EQUAL(3, p.get<TestProperty>().value);
	CHECK_EQUAL(3, constp.get<TestProperty>().value);
}

TEST(PropertyMapString)
{
	PropertyMap p;
	const PropertyMap& constp = p;
	p.set(TestProperty{4});
	CHECK_EQUAL(string(L"{SuitePropertyMap::TestProperty: Test property 4}"), toString(constp));
}

class ConstructedProperty: public PropertyMap::Property
{
public:
	explicit ConstructedProperty(int code) { }
	
	bool operator==(const ConstructedProperty& other) const {
		return true;
	}
	
	friend std::wostream& operator<<(std::wostream& out, const ConstructedProperty& property);
	virtual void print(std::wostream& out) const override { out << *this; }
};

inline std::wostream& operator<<(std::wostream& out, const ConstructedProperty& property)
{
	return out << L"Constructed property";
}

inline std::ostream& operator<<(std::ostream& out, const ConstructedProperty& property)
{
	return out << "Constructed property";
}

TEST(NoDefaultConstructor)
{
	PropertyMap p;
	CHECK_EQUAL(false, p.has<ConstructedProperty>());
	ConstructedProperty cp(123);
	CHECK_EQUAL(false, p.has<ConstructedProperty>());
	p.set(cp);
	CHECK_EQUAL(true, p.has<ConstructedProperty>());
	CHECK_EQUAL(cp, p.get<ConstructedProperty>());
}

} // SUITE
