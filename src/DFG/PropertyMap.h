#pragma once
#include "fixups.h"
#include "DFG/PropertyType.h"
#include "DFG/Property.h"

class PropertyMap
{
public:
	~PropertyMap();
	
	void set(const Property& prop);
	bool has(const PropertyType type) const;
	const Property* get(const PropertyType type) const;
	template<class T> bool has() const;
	template<class T> const T& get() const;
	
	void printProperties(std::wostream& out) const;
	
protected:
	std::map<PropertyType, Property*> _propertyMap;
};

inline std::wostream& operator<<(std::wostream& out, const PropertyMap& map)
{
	map.printProperties(out);
	return out;
}

template<class T> bool PropertyMap::has() const
{
	return has(T::classType);
}

template<class T> const T& PropertyMap::get() const
{
	const Property* prop = get(T::classType);
	const T* propT = dynamic_cast<const T*>(prop);
	if(propT == 0) throw L"Invalid propery map";
	return *propT;
}

