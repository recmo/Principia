#pragma once
#include <Unicode/string.h>
#include <Utilities/containers.h>
#include <Utilities/string.h>
#include <Utilities/memory.h>
#include <Utilities/pretty_print.h>
#include <map>
#include <memory>
#include <iostream>
#include <type_traits>

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

std::wostream& operator<<(std::wostream& out, const PropertyMap& map);
std::wostream& operator<<(std::wostream& out,
	const PropertyMap::Property& property);

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
