#include <Unicode/string.h>
#include <Utilities/containers.h>
#include <Utilities/memory.h>
#include <Utilities/assert.h>
#include <Utilities/pretty_print.h>
#include <Utilities/string.h>
#include <map>
#include <memory>
#include <type_traits>
#include <UnitTest++/UnitTest++.h>

class PropertyMap
{
public:
	class Property {
	public:
		virtual ~Property() { }
		virtual void print(std::wostream& out) const = 0;
	};
	
	template<class T> bool has() const;
	template<class T> const T& get() const;
	template<class T> T& get();
	template<class T> void set(const T& prop);
	
private:
	friend std::wostream& operator<<(std::wostream& out, const PropertyMap& map);
	std::map<string, std::unique_ptr<Property>> _map;
};

inline std::wostream& operator<<(std::wostream& out, const PropertyMap& map)
{
	return out << map._map;
}

inline std::wostream& operator<<(std::wostream& out, const PropertyMap::Property& property)
{
	property.print(out);
	return out;
}

template<class T>
bool PropertyMap::has() const
{
	static_assert(std::is_base_of<Property, T>::value,
		"Type must inherit from PropertyMap::Property");
	static const string key = typeName<T>();
	
	if(!contains(_map, key))
		return false;
	
	// The name exists, but check that it is of the requested type
	// This should succeed unless we have name collisions.
	if(dynamic_cast<const T*>(_map.at(key).get()) == nullptr)
		throw std::logic_error{"Name collision in property classes."};
	return true;
}

template<class T>
void PropertyMap::set(const T& prop)
{
	static_assert(std::is_base_of<Property, T>::value,
		"Type must inherit from PropertyMap::Property");
	static const string key = typeName<T>();
	
	// Call has<T>() to run the name collision check
	has<T>();
	
	// Store the new value
	_map[key] = std::make_unique<T>(prop);
}

template<class T>
const T& PropertyMap::get() const
{
	static_assert(std::is_base_of<Property, T>::value,
		"Type must inherit from PropertyMap::Property");
	static const string key = typeName<T>();
	
	if(!contains(_map, key))
		throw not_found{};
	
	const std::unique_ptr<Property>& ptr = _map.at(key);
	if(dynamic_cast<const T*>(ptr.get()) == nullptr)
		throw std::logic_error{"Name collision in property classes."};
	return *dynamic_cast<const T*>(ptr.get());
}

template<class T>
T& PropertyMap::get()
{
	static_assert(std::is_base_of<Property, T>::value,
		"Type must inherit from PropertyMap::Property");
	static const string key = typeName<T>();
	
	if(!contains(_map, key))
		throw not_found{};
	
	std::unique_ptr<Property>& ptr = _map.at(key);
	if(dynamic_cast<T*>(ptr.get()) == nullptr)
		throw std::logic_error{"Name collision in property classes."};
	return *dynamic_cast<T*>(ptr.get());
}


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
