#include <Unicode/string.h>
#include <Utilities/containers.h>
#include <Utilities/memory.h>
#include <Utilities/assert.h>
#include <Utilities/pretty_print.h>
#include <Utilities/string.h>
#include <map>
#include <memory>
#include <UnitTest++/UnitTest++.h>

class PropertyMap
{
public:
	class Property {
	public:
		virtual ~Property() { }
		string name() const { return _name; };
		virtual void print(std::wostream& out) const = 0;
		
	protected:
		explicit Property(string name) : _name(std::move(name)) { }
		
	private:
		const string _name;
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

inline std::wostream& operator<<(std::wostream& out, const PropertyMap::Property& propterty)
{
	propterty.print(out);
	return out;
}

template<class T>
bool PropertyMap::has() const
{
	if(!contains(_map, T::name))
		return false;
	
	// The name exists, but check that it is of the requested type
	// This should succeed unless we have name collisions.
	if(dynamic_cast<const T*>(_map.at(T::name).get()) == nullptr)
		throw std::logic_error{"Name collision in property classes."};
	return true;
}

template<class T>
void PropertyMap::set(const T& prop)
{
	const Property& base = static_cast<const Property&>(prop);
	if(base.name() != T::name) {
		throw std::logic_error("Static name differs from name"
			" passed to Property constructor.");
	}
	
	// Call has<T>() to run the name collision check
	has<T>();
	
	// Store the new value
	_map[T::name] = std::make_unique<T>(prop);
}

template<class T>
const T& PropertyMap::get() const
{
	if(!contains(_map, T::name))
		throw not_found{};
	
	const std::unique_ptr<Property>& ptr = _map.at(T::name);
	if(dynamic_cast<const T*>(ptr.get()) == nullptr)
		throw std::logic_error{"Name collision in property classes."};
	return *dynamic_cast<const T*>(ptr.get());
}

template<class T>
T& PropertyMap::get()
{
	if(!contains(_map, T::name))
		throw not_found{};
	
	std::unique_ptr<Property>& ptr = _map.at(T::name);
	if(dynamic_cast<T*>(ptr.get()) == nullptr)
		throw std::logic_error{"Name collision in property classes."};
	return *dynamic_cast<T*>(ptr.get());
}


SUITE(PropertyMap) {

class TestProperty: public PropertyMap::Property
{
public:
	static const string name;
	TestProperty(): Property(name) { }
	TestProperty(int v): Property(name), value(v) { }
	
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

const string TestProperty::name{L"TestProperty"};

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
	CHECK_EQUAL(string(L"{TestProperty: Test property 4}"), toString(constp));
}

class ConstructedProperty: public PropertyMap::Property
{
public:
	static const string name;
	explicit ConstructedProperty(int code): Property(name) { }
	
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

const string ConstructedProperty::name{L"ConstructedProperty"};

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

class WrongProperty: public PropertyMap::Property
{
public:
	static const string name;
	WrongProperty(): Property(L"WrongName") { }
	
	friend std::wostream& operator<<(std::wostream& out, const WrongProperty& property);
	virtual void print(std::wostream& out) const override { out << *this; }
};

inline std::wostream& operator<<(std::wostream& out, const WrongProperty& property)
{
	return out << L"Collide property";
}

const string WrongProperty::name{L"WrongProperty"};

TEST(NameFailure)
{
	PropertyMap p;
	WrongProperty wp;
	CHECK_THROW(p.set(wp), std::logic_error);
}

class CollideProperty: public PropertyMap::Property
{
public:
	static const string name;
	CollideProperty(): Property(name) { }
	
	friend std::wostream& operator<<(std::wostream& out, const CollideProperty& property);
	virtual void print(std::wostream& out) const override { out << *this; }
};

inline std::wostream& operator<<(std::wostream& out, const CollideProperty& property)
{
	return out << L"Collide property";
}

const string CollideProperty::name{L"TestProperty"};

TEST(NameCollision)
{
	PropertyMap p;
	TestProperty tp;
	p.set(tp);
	CHECK_EQUAL(true, p.has<TestProperty>());
	
	CollideProperty cp;
	CHECK_THROW(p.has<CollideProperty>(), std::logic_error);
	CHECK_THROW(p.set(cp), std::logic_error);
	CHECK_THROW(p.get<CollideProperty>(), std::logic_error);
}

} // SUITE
